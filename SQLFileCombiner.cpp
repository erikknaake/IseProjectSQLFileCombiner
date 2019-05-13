#include <iostream>
#include <experimental/filesystem>
#include <fstream>
#include <string>

namespace fs = std::experimental::filesystem;

int main()
{
	// Output file, clear before use
	std::ofstream testOutput("Root/testFile.sql", std::fstream::trunc);
	std::ofstream sqlOutput("Root/sqlFile.sql", std::fstream::trunc);

	std::fstream createFile("Root/create.sql");
	sqlOutput << createFile.rdbuf();

	for (auto& file : fs::recursive_directory_iterator("Root"))
	{
		std::cout << file.path().u8string() << std::endl;
		if (file.path().u8string().find(".spec") != std::string::npos)
		{
			std::string line;
			std::fstream currentFile(file);

			if (!currentFile.is_open())
			{
				std::cout << file.path() << " could not be opened" << std::endl;
				break;
			}
			while (std::getline(currentFile, line))
			{
				if (line.find("EXEC tSQLt.Run") == std::string::npos)
					testOutput << line << std::endl;
			}
		}
		else if (file.path().u8string().find("sqlFile.sql") == std::string::npos && file.path().u8string().find("create.sql") == std::string::npos)
			sqlOutput << std::fstream(file).rdbuf();
	}

	testOutput << "\nEXEC tSQLt.RunAll\n";
}