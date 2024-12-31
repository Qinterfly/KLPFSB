
## About
The program is aimed at swapping audio files in WormsWMD. It uses some functionality of [vgmstream](https://github.com/vgmstream/vgmstream/tree/master) to manipulate content of "*.fsb" files

## Usage
1. The original sound banks are located in the following directory: "WormsWMD/Audio/PC". For example, choose “Speech_RussianMale.fsb”.
2. Open this file with the program [FsbExtractor.exe](http://www.aezay.dk/aezay/fsbextractor/) and extract the audio files to a separate Music directory.
3. Replace the sounds with your own in the Music directory. [VLC Player](https://www.videolan.org/vlc/) is recommended to use as to convert the files. The sounds have to meet the following requirements:
	* Format: MP3;
	* Codec: MPEG Audio;
	* Bitrate: 128;
	* Channel: 1;
	* Frequency: 44100.

4. Run [KLPFSB.exe](KLPFSB.exe) specifying options according to help:
	- -e : Extension of audio files
	- -i : Path to the original .fsb file to be used as a prototype
	- -m : Path to the directory which contains audio files for swapping
	- -o : Path to the output .fsb file

Example of usage:

	* ./KLPFSB.exe -e ".mp3" -i "examples/input.fsb" -m "examples/music" -o "examples/output.fsb"
	
5. Copy the modified file back to the "WormsWMD/Audio/PC" directory.

*Do not forget to do the same operations with "*_Radio.fsb" files as well to change all original sound banks.*