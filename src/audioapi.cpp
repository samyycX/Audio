#include "audioapi.h"
#include "globals.h"
#include "helper.h"
#include <iostream>
#include <thread>
#include <shared_mutex>
#include <string>

// void SetPlayerVolume(int slot, float factor)
// {
//   std::unique_lock<std::shared_mutex> lock(g_Mutex);
//   g_PlayerVolume[slot] = factor;
// }

// void SetAllPlayerVolume(float factor)
// {
//   std::unique_lock<std::shared_mutex> lock(g_Mutex);
//   for (int i = 0; i < MAX_SLOT; i++)
//   {
//     g_PlayerVolume[i] = factor;
//   }
// }

// float GetPlayerVolume(int slot)
// {
//   std::shared_lock<std::shared_mutex> lock(g_Mutex);
//   return g_PlayerVolume[slot];
// }

void SetPlayerHearing(int slot, bool hearing)
{
  std::unique_lock<std::shared_mutex> lock(g_Mutex);
  g_PlayerHearing[slot] = hearing;
}

void SetAllPlayerHearing(bool hearing)
{
  std::unique_lock<std::shared_mutex> lock(g_Mutex);
  for (int i = 0; i < MAX_SLOT; i++)
  {
    g_PlayerHearing[i] = hearing;
  }
}

bool IsHearing(int slot)
{
  std::shared_lock<std::shared_mutex> lock(g_Mutex);
  return g_PlayerHearing[slot];
}
void SetPlayerAudioBufferString(int slot, std::string audioBuffer, std::string audioPath)
{
  if (audioBuffer.size() == 0 && audioPath.size() == 0)
  {
    std::unique_lock<std::shared_mutex> lock(g_Mutex);
    g_PlayerAudioBuffer[slot].clear();
    return;
  }
  auto lambda = [slot](std::vector<SVCVoiceDataMessage> msgbuffer)
  {
    std::unique_lock<std::shared_mutex> lock(g_Mutex);
    g_PlayerAudioBuffer[slot] = msgbuffer;
  };
  std::thread process(ProcessVoiceData, audioBuffer, audioPath, 1, lambda);
  process.detach();
}

void SetAllAudioBufferString(std::string audioBuffer, std::string audioPath)
{
  if (audioBuffer.size() == 0 && audioPath.size() == 0)
  {
    std::unique_lock<std::shared_mutex> lock(g_Mutex);
    g_GlobalAudioBuffer.clear();
    return;
  }
  auto lambda = [](std::vector<SVCVoiceDataMessage> msgbuffer)
  {
    std::unique_lock<std::shared_mutex> lock(g_Mutex);
    g_GlobalAudioBuffer = msgbuffer;
  };
  std::thread process(ProcessVoiceData, audioBuffer, audioPath, 1, lambda);
  process.detach();
}

bool IsPlaying(int slot)
{
  std::shared_lock<std::shared_mutex> lock(g_Mutex);
  return g_PlayerAudioBuffer[slot].size() > 0;
}

bool IsAllPlaying()
{
  std::shared_lock<std::shared_mutex> lock(g_Mutex);
  return g_GlobalAudioBuffer.size() > 0;
}

// void CAudioPlayerInterface::SetPlayerVolume(int slot, float factor)
// {
//   SetPlayerVolume(slot, factor);
// }

// void CAudioPlayerInterface::SetAllPlayerVolume(float factor)
// {
//   SetAllPlayerVolume(factor);
// }
// float CAudioPlayerInterface::GetPlayerVolume(int slot)
// {
//   return GetPlayerVolume(slot);
// }
void CAudioPlayerInterface::SetPlayerHearing(int slot, bool hearing)
{
  SetPlayerHearing(slot, hearing);
}
void CAudioPlayerInterface::SetAllPlayerHearing(bool hearing)
{
  SetAllPlayerHearing(hearing);
}
bool CAudioPlayerInterface::IsHearing(int slot)
{
  return IsHearing(slot);
}
void CAudioPlayerInterface::SetPlayerAudioBuffer(int slot, const char *audioBuffer, int audioBufferSize)
{
  if (audioBufferSize == 0 || audioBuffer == nullptr)
  {
    SetPlayerAudioBufferString(slot, "", "");
    return;
  }
  SetPlayerAudioBufferString(slot, std::string(audioBuffer, audioBufferSize), "");
}
void CAudioPlayerInterface::SetPlayerAudioFile(int slot, const char *audioFile, int audioFileSize)
{
  if (audioFileSize == 0 || audioFile == nullptr)
  {
    SetPlayerAudioBufferString(slot, "", "");
    return;
  }
  SetPlayerAudioBufferString(slot, "", std::string(audioFile, audioFileSize));
}

void CAudioPlayerInterface::SetAllAudioBuffer(const char *audioBuffer, int audioBufferSize)
{
  if (audioBufferSize == 0 || audioBuffer == nullptr)
  {
    SetAllAudioBufferString("", "");
    return;
  }
  SetAllAudioBufferString(std::string(audioBuffer, audioBufferSize), "");
}
void CAudioPlayerInterface::SetAllAudioFile(const char *audioFile, int audioFileSize)
{
  if (audioFileSize == 0 || audioFile == nullptr)
  {
    SetAllAudioBufferString("", "");
    return;
  }
  SetAllAudioBufferString("", std::string(audioFile, audioFileSize));
}
bool CAudioPlayerInterface::IsPlaying(int slot)
{
  return IsPlaying(slot);
}
bool CAudioPlayerInterface::IsAllPlaying()
{
  return IsAllPlaying();
}

// void NativeSetPlayerVolume(int slot, float factor)
// {
//   SetPlayerVolume(slot, factor);
// }

// void NativeSetAllPlayerVolume(float factor)
// {
//   SetAllPlayerVolume(factor);
// }

// float NativeGetPlayerVolume(int slot)
// {
//   return GetPlayerVolume(slot);
// }

extern "C"
{
  void __cdecl NativeSetPlayerHearing(int slot, bool hearing)
  {
    SetPlayerHearing(slot, hearing);
  }

  void __cdecl NativeSetAllPlayerHearing(bool hearing)
  {
    SetAllPlayerHearing(hearing);
  }

  bool __cdecl NativeIsHearing(int slot)
  {
    return IsHearing(slot);
  }

  void __cdecl NativeSetPlayerAudioBufferString(int slot, const char *audioBuffer, int audioBufferSize, const char *audioPath, int audioPathSize)
  {
    auto data1 = std::string(audioBuffer, audioBufferSize);
    auto data2 = std::string(audioPath, audioPathSize);

    SetPlayerAudioBufferString(slot, data1, data2);
  }

  void __cdecl NativeSetAllAudioBufferString(const char *audioBuffer, int audioBufferSize, const char *audioPath, int audioPathSize)
  {
    auto data1 = std::string(audioBuffer, audioBufferSize);
    auto data2 = std::string(audioPath, audioPathSize);

    SetAllAudioBufferString(data1, data2);
  }

  bool __cdecl NativeIsPlaying(int slot)
  {
    return IsPlaying(slot);
  }

  bool __cdecl NativeIsAllPlaying()
  {
    return IsAllPlaying();
  }
}