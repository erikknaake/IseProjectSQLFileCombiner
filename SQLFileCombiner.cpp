#include <iostream>
#include <experimental/filesystem>
#include <fstream>
#include <string>
#include <json/json.hpp>
#include <regex>
#include <map>

namespace fs = std::experimental::filesystem;
using json = nlohmann::json;

bool matchRegex(const json* reg, std::string str)
{
	if (reg->type() == json::value_t::array)
	{
		bool matches = true;
		for (auto i : *reg)
		{
			matches = matches && matchRegex(&i, str);
		}
		return matches;
	}
	if (reg->type() == json::value_t::object)
	{
		return (*reg)["invert"].get<bool>() ^ std::regex_match(str, std::regex((*reg)["regex"].get<std::string>()));
	}
	return std::regex_match(str, std::regex(reg->get<std::string>()));
}

bool skipFile(const json* config, const json* files, const fs::recursive_directory_iterator::value_type* file)
{
	for (auto ignoreFiles : (*config)["skipFiles"])
	{
		if (matchRegex(&ignoreFiles, file->path().u8string()))
		{
			return true;
		}
	}

	for (auto dependencies : (*config)["testDependencies"])
	{
		if (matchRegex(&dependencies, file->path().u8string()))
		{
			return true;
		}
	}

	for (auto allowFiles : *files)
	{
		if (matchRegex(&allowFiles, file->path().u8string()))
		{
			return false;
		}
	}
	return true;
}

template<typename Functor>
void forEveryFileInOrder(const json* config, const json* files, Functor functor)
{
	std::map<std::string, int> includedFiles;
	for (auto fileOrder : (*config)["fileOrder"])
	{
		for (auto file : fs::recursive_directory_iterator("./"))
		{
			
			if (includedFiles.find(file.path().u8string()) != includedFiles.end()
				|| skipFile(config, files, &file)
				|| file.path().u8string().find((*config)["testFile"].get<std::string>()) != std::string::npos
				|| file.path().u8string().find((*config)["outputFile"].get<std::string>()) != std::string::npos)
				continue;
			
			includedFiles[file.path().u8string()] = true;

			std::cout << file.path().u8string() << std::endl;
			functor(file);
		}
	}
}

void addFiletoFile(std::ofstream* output, const fs::recursive_directory_iterator::value_type* file, const json* config)
{
	std::fstream input(*file);
	std::string line;
	while (std::getline(input, line))
	{
		bool include = true;
		for (auto ignoreLines : (*config)["skipLines"])
		{
			if (std::regex_match(line, std::regex(ignoreLines.get<std::string>())))
			{
				include = false;
				break;
			}
		}
		if (include)
			*output << line << std::endl;
	}
	input.close();
}

int main()
{
	std::fstream configFile("parserConfig.json");
	json config = json::parse(configFile);
	configFile.close();
	std::ofstream testFile(config["testFile"].get<std::string>(), std::ios::trunc);
	std::ofstream outputFile(config["outputFile"].get<std::string>(), std::ios::trunc);

	for (auto i : config["addTestLines"]["start"])
		testFile << i.get<std::string>() << std::endl;

	for (auto i : config["testDependencies"])
	{
		for (auto file : fs::recursive_directory_iterator("./"))
		{
			if (matchRegex(&i, file.path().u8string()))
			{
				std::fstream dependency(file);
				testFile << dependency.rdbuf();
				dependency.close();
			}
		}
	}
		
	forEveryFileInOrder(&config, &config["testFiles"], [&testFile, &config](fs::recursive_directory_iterator::value_type file)
	{
		addFiletoFile(&testFile, &file, &config);
	});

	forEveryFileInOrder(&config, &config["standardFiles"], [&outputFile, &config](fs::recursive_directory_iterator::value_type file) 
	{
		addFiletoFile(&outputFile, &file, &config);
	});

	for (auto i : config["addTestLines"]["end"])
		testFile << i.get<std::string>() << std::endl;

	testFile.close();
	outputFile.close();
}