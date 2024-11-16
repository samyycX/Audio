#pragma once

#include <vector>

#define AUDIOPLAYER_INTERFACE "AudioPlayer002"

class IAudioPlayer
{
public:
  virtual bool PlayAudio(std::vector<uint8_t> audio_buffer, float voice_level) = 0;
};