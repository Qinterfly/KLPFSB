#include <iostream>

#include "fsb5.h"
#include "utility.h"

int main(int argc, char* argv[])
{
	// Initialize the parser
	CMDParser cmd;
	cmd.setArgument('m', "Path to the directory which contains audio files for swapping");
	cmd.setArgument('e', "Extension of audio files");
	cmd.setArgument('i', "Path to the original .fsb file to be used as a prototype");
	cmd.setArgument('o', "Path to the output .fsb file");
	cmd.printHelp();

	// Parse the arguments
	bool isParsed = cmd.parse(argc, argv);
	if (!isParsed)
	{
		printError("Could not parse command line arguments");
		return 1;
	}
	std::string pathMusic      = cmd.getValue('m', "examples/music");
	std::string extensionMusic = cmd.getValue('e', ".mp3");
	std::string pathInputFSB   = cmd.getValue('i', "examples/input.fsb");
	std::string pathOutputFSB  = cmd.getValue('o', "examples/output.fsb");

	// Display values of arguments
	std::cout << "Current run options: " << std::endl;
	std::cout << "* Music directory: " << pathMusic << std::endl;
	std::cout << "* Music files extension: " << extensionMusic << std::endl;
	std::cout << "* FSB input file: " << pathInputFSB << std::endl;
	std::cout << "* FSB output file: " << pathOutputFSB << std::endl;
	std::cout << std::endl;

	// Read the music files
	auto musicFiles = dirContent(pathMusic, extensionMusic);
	if (musicFiles.empty()) 
	{
		printError("Could not found the music files located at the specified directory: " + pathMusic);
		return 1;
	}
	auto music = readBinaryFiles(musicFiles);

	// Read the fsb prototype
	auto fsb = readBinaryFile(pathInputFSB);
	if (fsb.empty()) 
	{
		printError("Could not read the fsb file: " + pathInputFSB);
		return 1;
	}

	// Swap the music and write the fsb file
	int numSwap = swapMusic(fsb, music, pathOutputFSB);
	if (numSwap > 0)
		std::cout << "Number of swapped audio tracks is " << numSwap << std::endl;
	else
		std::cout << "Swapping of audio tracks is failed" << std::endl;
	std::cout << "The program has finished. Press any key to exit...";
	std::cin.get();
}