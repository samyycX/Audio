# Audio
A demo and a metamod lib to provide similar functions to the previous SM-Ext-Audio extension in csgo, that is, sending custom audio streams such as a song through a bot's voice chat.

## Table of Contents
- [Demo](#demo)
- [Prerequisites](#prerequisites)
- [LINUX USER MUST READ](#linux-user-must-read)
- [API Usage](#api-usage)
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

## API Usage
See [Wiki](https://github.com/samyycX/Audio/wiki/API-Usage)

## Crash / Not working
I don't have the ability to fully test this plugin, if you have any problem, please submit an issue and describe your problem as precise as you can.

## Credits
- **CS2Fixes**: Copied some codes from it.
- **multiaddonmanager**: Copied some codes from it.
- Many helps from **Poggu**.
