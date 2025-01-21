# Audio
A demo and a metamod lib to provide similar functions to the previous SM-Ext-Audio extension in csgo, that is, sending custom audio streams such as a song through a bot's voice chat.

## Table of Contents
- [Demo](#demo)
- [Prerequisites](#prerequisites)
- [LINUX USER MUST READ](#linux-user-must-read)
- [Metamod API Usage](#metamod-api-usage)
- [Counterstrikesharp API Usage](#counterstrikesharp-api-usage)
- [Crash / Not working](#crash--not-working)
- [Credits](#credits)

## Demo
*unmute the video*
<div><video controls src="https://github.com/user-attachments/assets/27ca1fd5-6ae7-4d1f-be66-aa0bbea2fa22"></video></div>

## Prerequisites
1. ffmpeg installed and can be found in your PATH.

## LINUX USER MUST READ
If you are using linux and want to use it in counterstrikesharp, after you installed the plugin you need to execute this (replace `{YOUR VERSION}` with your actual dir name)
```
ln -s /path/to/game/csgo/addons/audio/bin/audio.so /path/to/game/csgo/addons/counterstrikesharp/dotnet/shared/Microsoft.NETCore.App/{YOUR VERSION}/audio.so
```
in your server root directory.

## Metamod API Usage
1. Include the `iaudio.h` in `public` folder.
2. Query the interface in `AllPluginsLoaded` and use it like this:
```c++
// in AllPluginsLoaded
IAudio *pAudio = (IAudio*)g_SMAPI->MetaFactory(AUDIO_INTERFACE, nullptr, nullptr);

// wherever you like
std::string str = "D:/xxx.mp3";
pInterface->SetAllAudioFile(str.c_str(), str.size());
```
Further API methods are in the `iaudio.h` interface doc.

## Counterstrikesharp API Usage
1. Copy the `Audio.cs` in `public` folder to your project.
2. Use it.

Further API methods are in the `iaudio.h` interface doc.

## Crash / Not working
I don't have the ability to fully test this plugin, if you have any problem, please submit an issue and describe your problem as precise as you can.

## Credits
- **CS2Fixes**: Copied some codes from it.
- **multiaddonmanager**: Copied some codes from it.
- Many helps from **Poggu**.
