#include "audiocode.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <stdio.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#endif

std::string getTempDirectory()
{
#ifdef PLATFORM_WINDOWS
  char tempPath[MAX_PATH];
  GetTempPath(MAX_PATH, tempPath);
  return std::string(tempPath);
#else
  return "/tmp/";
#endif
}

std::vector<uint8_t> convertAudioBufferToPCM(std::string file_path)
{
  std::vector<uint8_t> outputBuffer;
  std::ostringstream command;
  command << "ffmpeg -y -i \"" << file_path << "\" -acodec pcm_s16be -ac 1 -ar 48000 -f s16be -";

#if defined(_WIN32)
  FILE *pipe = _popen(command.str().c_str(), "rb");
#else
  FILE *pipe = popen(command.str().c_str(), "r");
#endif

  if (!pipe)
  {
    throw std::runtime_error("Failed to run FFmpeg command");
  }

  uint8_t buffer[4096];
  size_t bytesRead;
  while ((bytesRead = fread(buffer, 1, sizeof(buffer), pipe)) > 0)
  {
    outputBuffer.insert(outputBuffer.end(), buffer, buffer + bytesRead);
  }

#if defined(_WIN32)
  _pclose(pipe);
#else
  pclose(pipe);
#endif

  return outputBuffer;
}