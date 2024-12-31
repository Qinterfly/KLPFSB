#pragma once

#include <string>
#include <vector>
#include <map>

#include "fsb5.h"

using Strings = std::vector<std::string>;
using Data = std::vector<char>;
using DataSet = std::map<std::string, Data>;
using ArgData = std::map<char, std::string>;

// Reading
Strings dirContent(std::string const& dir, std::string const& reqExt = "");
Data readBinaryFile(std::string const& path);
DataSet readBinaryFiles(Strings const& paths);

// Modifying *.fsb files
int swapMusic(Data& fsb, DataSet const& music, std::string const& pathOutFile);

// Command line parser
class CMDParser
{
public:
	CMDParser() = default;
	~CMDParser() = default;
	bool setArgument(char name, std::string const& definition);
	std::string getValue(char name, std::string const& defaultValue);
	bool parse(int argc, char* argv[]);
	void printHelp();

private:

	ArgData mArgDefinitions;
	ArgData mArgValues;
};

// Common
void printError(std::string const& message);