
#include <iostream>
#include <filesystem>
#include <fstream>
#include <algorithm>

#include "utility.h"

namespace fs = std::filesystem;

// Retrieve all files of the specified extension located in the directory
Strings dirContent(std::string const& dir, std::string const& reqExt)
{
	Strings result;

	if (!fs::exists(dir) || !fs::is_directory(dir))
		return result;

	for (const auto& entry : fs::directory_iterator(dir)) {

		struct stat sb;
		fs::path path = entry.path();
		if (!reqExt.empty() && path.extension() != reqExt)
			continue;
		std::string pathStr = path.string();
		const char* pathCStr = pathStr.data();
		if (stat(pathCStr, &sb) == 0 && !(sb.st_mode & S_IFDIR))
			result.push_back(pathCStr);
	}
	
	return result;
}

// Retrieve content of the binary file
Data readBinaryFile(std::string const& path)
{
	std::vector<char> result;

	if (!fs::exists(path))
		return result;
	std::ifstream file(path, std::ios::binary);

	// Get the file size
	file.seekg(0, std::ios_base::end);
	size_t length = file.tellg();
	file.seekg(0, std::ios_base::beg);

	// Retrieve its content
	result.reserve(length);
	std::copy(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), std::back_inserter(result));

	return result;
}

// Read a set of binary files
DataSet readBinaryFiles(Strings const& paths)
{
	DataSet result;
	for (std::string const& path : paths)
	{
		std::string fileName = fs::path(path).stem().string();
		result[fileName] = readBinaryFile(path);
	}
	return result;
}

// Change the audio data of the .fsb file
int swapMusic(Data& fsb, DataSet const& music, std::string const& pathOutFile)
{
	int numSwap = 0;

	// Parse the .fsb content
	Fsb5Header* headers = nullptr;
	auto fsbData = (unsigned char*)fsb.data();
	bool isRead = parseFsb(fsbData, &headers) == 0;
	if (!isRead)
	{
		std::cout << "Could not parse the *.fsb file" << std::endl;
		return -1;
	}

	// Change offsets and number of samples
	uint32_t offset = 0;
	int numSubsongs = headers->total_subsongs;
	std::vector<int> offsets(numSubsongs, 0);
	std::vector<int> sizes(numSubsongs, 0);
	int totalSize = 0;
	std::cout << "Processing audio files:" << std::endl;
	for (int i = 0; i != numSubsongs; ++i)
	{
		Fsb5Header* pHeader = headers + i;
		std::string name = (char*)&pHeader->songName;

		// Get the audio length
		int size = 0;
		if (music.contains(name))
		{
			auto pIter = music.find(name);
			size = pIter->second.size();
		}
		else
		{
			std::cout << "The audio is not found in the prototype .fsb file: " << name << std::endl;
			size = pHeader->stream_size;
		}
		uint32_t numSamples = (uint32_t)((size * 8.0 * pHeader->sample_rate) / pHeader->bitrate);

		// Align the audio length
		size = ((size - 1) / 32 + 1) * 32;
		
		// Modify the fsb header
		setNumSamples(pHeader->ptrStart, numSamples);
		offset += size;
		if (i != numSubsongs - 1)
		{
			setOffset(pHeader->ptrOffset, offset);
			offsets[i] = offset;
		}

		// Print the substituted audio name
		numSwap += 1;
		std::cout << numSwap << " - " << name << std::endl;

		// Increase the counters
		sizes[i] = size;
		totalSize += size;
	}
	*headers->ss = totalSize;

	// Create the file for writing
	std::ofstream file(pathOutFile, std::ios::binary);
	if (file.is_open())
	{
		std::cout << "Writing the output file: " << pathOutFile << std::endl;
	}
	else
	{
		std::cout << "Could not open the output .fsb file: " << pathOutFile << std::endl;
		return -1;
	}

	// Write the headers
	for (int i = 0; i != headers->stream_offset; ++i)
		file << fsbData[i];

	// Append the audio data to the file
	for (int i = 0; i != numSubsongs; ++i)
	{
		Fsb5Header* pHeader = headers + i;
		std::string name = (char*)&pHeader->songName;
		unsigned char* data;
		int size;

		// Retrieve the data for writing
		if (music.contains(name))
		{
			auto pIter = music.find(name);
			Data const& value = pIter->second;
			data = (unsigned char*)value.data();
			size = (int)value.size();
		}
		else
		{
			data = &fsbData[pHeader->stream_offset];
			size = pHeader->stream_size;
		}

		// Wrtie the audio content
		for (int j = 0; j != size; ++j)
			file << data[j];
		for (int j = size; j < sizes[i]; ++j)
			file << '\0';
	}

	return numSwap;
}

// Add a new argument to parse
bool CMDParser::setArgument(char name, std::string const& definition)
{
	if (mArgDefinitions.contains(name))
	{
		std::cout << "The command line argument is already specified: " << name << std::endl;
		return false;
	}
	mArgDefinitions[name] = definition;
	return true;
}

// Retrieve the argument value
std::string CMDParser::getValue(char name, std::string const& defaultValue) 
{
	return mArgValues.contains(name) ? mArgValues[name] : defaultValue;
}

// Parse command line arguments
bool CMDParser::parse(int argc, char* argv[])
{
	int iArg = 1;
	while (iArg < argc)
	{
		std::string value = argv[iArg];
		value.erase(std::remove(value.begin(), value.end(), '-'), value.end());
		if (!value.empty()) 
		{
			char arg = value[0];
			if (mArgDefinitions.contains(arg))
			{
				++iArg;
				mArgValues[arg] = argv[iArg];
			}
		}
		++iArg;
	}
	return true;
}

// Display definitions of command line arguments
void CMDParser::printHelp() 
{
	std::cout << "Available options:" << std::endl;
	for (auto const& [key, value] : mArgDefinitions)
		std::cout << '-' << key << '\t' << value << std::endl;
	std::cout << std::endl;
}

void printError(std::string const& message) 
{
	std::cout << message << std::endl;
	std::cin.get();
}