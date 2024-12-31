
[![en](https://img.shields.io/badge/lang-en-red.svg)](https://github.com/Qinterfly/KLPFSB/master/README.md)
[![ru](https://img.shields.io/badge/lang-ru-red.svg)](https://github.com/Qinterfly/KLPFSB/master/README.ru.md)

## О программе
Программа предназначена для замены аудио файлов в WormWMD. Она использует некоторый функционал [vgmstream](https://github.com/vgmstream/vgmstream/tree/master) для изменения содержимого "*.fsb" файлов

## Использование
1. Исходные банки звуков расположены в следующей директории: "WormsWMD/Audio/PC". Для примера выберем “Speech_RussianMale.fsb”.
2. Откройте этот файл посредством программы [FsbExtractor.exe](http://www.aezay.dk/aezay/fsbextractor/) (он размещена в директории "extra") и извлеките его содержимое в отдельную директорию (Music).
3. Замените файлы в директории Music. Рекомендуем использовать [VLC Player](https://www.videolan.org/vlc/) для конвертации файлов. Они должны отвечать следующим требованиям:
	* Формат: MP3;
	* Кодек: MPEG Audio;
	* Битрейт: 128;
	* Канал: 1;
	* Частота: 44100.

4. Используйте [KLPFSB.exe](KLPFSB.exe) задавая следующие опции:
	- -e : Расширение аудо файлов;
	- -i : Путь до ".fsb" файла, содержащего оригинальные аудио файлы, которые впоследствии используется в качестве прототипа;
	- -m : Путь до директории с аудио файлами для замены;
	- -o : Путь до результирующего ".fsb" файла.

Пример использования:

	* ./KLPFSB.exe -e ".mp3" -i "examples/input.fsb" -m "examples/music" -o "examples/output.fsb"
	
5. Скопируйте результирующий ".fsb" файл в директорию "WormsWMD/Audio/PC".

*Не забудьте осуществить те же операции с файлами "\*_Radio.fsb" для замены всех оригинальных банков звуков.*