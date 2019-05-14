#include <iostream>
#include <experimental/filesystem>
#include <fstream>
#include <string>
#include <map>

namespace fs = std::experimental::filesystem;

int main()
{
	// Output file, clear before use
	const char* sqlPath = "sqlFile.sql";
	const char* createScript = "create.sql";
	const char* testFile = "testFile.sql";
	const char* use = "USE IseProject";

	std::ofstream testOutput(testFile, std::fstream::trunc);
	std::ofstream sqlOutput(sqlPath, std::fstream::trunc);
	std::fstream createFile(createScript);

	sqlOutput << createFile.rdbuf();
	testOutput << "USE IseProject\n";

	for (auto& file : fs::recursive_directory_iterator("./"))
	{
		if (file.path().u8string().find(".sql") == std::string::npos)
			continue;
		std::cout << file.path().u8string() << std::endl;

		std::string line;
		std::fstream currentFile(file);

		if (!currentFile.is_open())
		{
			std::cout << file.path() << " could not be opened" << std::endl;
			break;
		}

		if (file.path().u8string().find(".spec") != std::string::npos)
		{
			while (std::getline(currentFile, line))
			{
				if (line.find("EXEC tSQLt.Run") == std::string::npos && line.find(use) == std::string::npos)
					testOutput << line << std::endl;
			}
		}
		else if (file.path().u8string().find(sqlPath) == std::string::npos 
			&& file.path().u8string().find(createScript) == std::string::npos
			&& file.path().u8string().find(testFile) == std::string::npos)
		{
			while (std::getline(currentFile, line))
			{
				if (line.find(use) == std::string::npos)
					sqlOutput << line << std::endl;
			}
		}	
	}

	testOutput << "\nEXEC tSQLt.RunAll\n";
	sqlOutput.close();
	testOutput.close();
}