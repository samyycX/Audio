#pragma once

#include <vector>
#include <iostream>

#define AUDIOPLAYER_INTERFACE "AudioPlayer002"

class IAudioPlayer
{
public:
  virtual bool PlayAudio(std::string filename, float voice_level) = 0;
};