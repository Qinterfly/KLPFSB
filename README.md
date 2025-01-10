
[![en](https://img.shields.io/badge/lang-en-red.svg)](https://github.com/Qinterfly/KLPFSB/blob/master/README.md)
[![ru](https://img.shields.io/badge/lang-ru-green.svg)](https://github.com/Qinterfly/KLPFSB/blob/master/README.ru.md)

## About
The program is aimed at swapping audio files in WormsWMD. It uses some functionality of [vgmstream](https://github.com/vgmstream/vgmstream/tree/master) to manipulate content of "*.fsb" files.

## Program usage
1. Download the latest program [release](https://github.com/Qinterfly/KLPFSB/releases/tag/v.0.0.1) and unpack it.
2. The original sound banks are located in the following directory: "WormsWMD/Audio/PC". For example, choose “Speech_RussianMale.fsb”.
3. Open this file with the program [FsbExtractor.exe](http://www.aezay.dk/aezay/fsbextractor/) (it is contained in the "extra" directory) and extract the audio files to a separate *Music* directory.
4. Replace the sounds with your own in the Music directory. [VLC Player](https://www.videolan.org/vlc/) is recommended to use as to convert the files. The sounds have to meet the following requirements:
	* Format: MP3;
	* Codec: MPEG Audio;
	* Bitrate: 128 bit/s;
	* Channel: 1;
	* Sample rate: 44100 Hz.

5. Run [KLPFSB.exe](KLPFSB.exe) specifying options according to help:
	- -e : Extension of audio files;
	- -i : Path to the original .fsb file to be used as a prototype;
	- -m : Path to the directory which contains audio files for swapping;
	- -o : Path to the output .fsb file.

Example of usage with arguments:
	
	./KLPFSB.exe -e ".mp3" -i "examples/input.fsb" -m "examples/music" -o "examples/output.fsb"
	
Alternatively, the program could be used without command line arguments. In that case the following steps actions have to be taken:
* Move audio files to the directory "examples/music";
* Place the original sound bank file in the following path: "examples/input.fsb".

In that scenario the program will create the file "examples/output.fsb" which has to be renamed and copied according to the next step.
	
6. Copy the modified file back to the "WormsWMD/Audio/PC" directory.

*Do not forget to do the same operations with "\*_Radio.fsb" files as well to change all original sound banks.*

## Specifics of sound banks in WormsWMD
While loading sound banks in WormsWMD hash totals are checked. Algorithm of their generation is unknown. So, the generation of '\*.fsb' files by means of [FMOD SoundBank Generator](https://www.fmod.com/) results in the game becomes silent. That leads to the necessity of substituting audio files while saving the most of the original header, which is essential for WormsWMD.