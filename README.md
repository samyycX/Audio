# AudioPlayer
A demo and a metamod lib to provide similar functions to the previous SM-Ext-Audio extension in csgo, that is, sending custom audio streams such as a song through a bot's voice chat.

> [!WARNING]
> Will auto enable `sv_alltalk 1`.

## Prerequisites
1. ffmpeg

## API Usage
1. Include the `iaudioplayer.h` in `public` folder.
2. Query the interface in `AllPluginsLoaded` and use it like this:
```c++
// in AllPluginsLoaded
IAudioPlayer *pAudioPlayer = (IAudioPlayer*)g_SMAPI->MetaFactory(AUDIOPLAYER_INTERFACE, nullptr, nullptr);

// wherever you like
pAudioPlayer->PlayAudio(std::string file_path, float voice_level);
```

### `IAudioPlayer::PlayAudio`
Params:
- `file_path`: Path to any audio file that can be decoded by ffmpeg is fine, such as mp3, wav
- `voice_level`: Basically it's voice volume, most of the time it's negative.

## Credits
- **CS2Fixes**: Copied some codes from it.
- **multiaddonmanager**: Copied some codes from it.
- Many helps from **Poggu**.