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
	const char* use = "USE BPMN_DB";
	const char* clrEnablePath = "SetClrEnabled.sql";
	const char* tSQLtClassPath = "tSQLt.class.sql";

	std::ofstream testOutput(testFile, std::fstream::trunc);
	std::ofstream sqlOutput(sqlPath, std::fstream::trunc);
	std::fstream tempFile(createScript);

	sqlOutput << tempFile.rdbuf() << std::endl;

	testOutput << use << std::endl;
	tempFile.close();
	tempFile.open(clrEnablePath);
	testOutput << tempFile.rdbuf();
	tempFile.close();
	tempFile.open(tSQLtClassPath);
	testOutput << tempFile.rdbuf() << std::endl;

	for (auto& file : fs::recursive_directory_iterator("./"))
	{
		if (file.path().u8string().find(".sql") == std::string::npos
			|| file.path().u8string().find(clrEnablePath) != std::string::npos
			|| file.path().u8string().find(tSQLtClassPath) != std::string::npos
			|| file.path().u8string().find(sqlPath) != std::string::npos
			|| file.path().u8string().find(createScript) != std::string::npos
			|| file.path().u8string().find(testFile) != std::string::npos)
			continue;
		std::cout << file.path().u8string() << std::endl;

		std::string line;
		std::fstream currentFile(file);

		if (!currentFile.is_open())
		{
			std::cout << file.path() << " could not be opened" << std::endl;
			getchar();
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
		else
		{
			while (std::getline(currentFile, line))
			{
				if (line.find(use) == std::string::npos)
					sqlOutput << line << std::endl;
			}
		}	
		currentFile.close();
	}

	testOutput << "\nEXEC tSQLt.RunAll\n";
	sqlOutput.close();
	testOutput.close();
}