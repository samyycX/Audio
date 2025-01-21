#include "api.h"

bool g_Initialized = false;
std::vector<luabridge::LuaRef> g_PlayStartListeners;
std::vector<luabridge::LuaRef> g_PlayEndListeners;

void Audio::SetPlayerHearing(int slot, bool hearing)
{
  m_pAudioPlayer->SetPlayerHearing(slot, hearing);
}

void Audio::SetAllPlayerHearing(bool hearing)
{
  m_pAudioPlayer->SetAllPlayerHearing(hearing);
}

bool Audio::IsHearing(int slot)
{
  return m_pAudioPlayer->IsHearing(slot);
}

void Audio::SetPlayerAudioBuffer(int slot, std::string audioBuffer)
{
  m_pAudioPlayer->SetPlayerAudioBuffer(slot, audioBuffer.c_str(), audioBuffer.size());
}

void Audio::SetPlayerAudioFile(int slot, std::string audioFile)
{
  m_pAudioPlayer->SetPlayerAudioFile(slot, audioFile.c_str(), audioFile.size());
}

void Audio::SetAllAudioBuffer(std::string audioBuffer)
{
  g_SMAPI->ConPrintf("%d\n", audioBuffer.size());
  m_pAudioPlayer->SetAllAudioBuffer(audioBuffer.c_str(), audioBuffer.size());
}

void Audio::SetAllAudioFile(std::string audioFile)
{
  m_pAudioPlayer->SetAllAudioFile(audioFile.c_str(), audioFile.size());
}

bool Audio::IsPlaying(int slot)
{
  return m_pAudioPlayer->IsPlaying(slot);
}

bool Audio::IsAllPlaying()
{
  return m_pAudioPlayer->IsAllPlaying();
}

void Audio::RegisterPlayStartListener(luabridge::LuaRef handler)
{
  m_PlayStartListeners.push_back(handler);
  g_PlayStartListeners.push_back(handler);
}

void Audio::UnregisterPlayStartListener(luabridge::LuaRef handler)
{
  m_PlayStartListeners.erase(std::remove(m_PlayStartListeners.begin(), m_PlayStartListeners.end(), handler), m_PlayStartListeners.end());
  g_PlayStartListeners.erase(std::remove(g_PlayStartListeners.begin(), g_PlayStartListeners.end(), handler), g_PlayStartListeners.end());
}

void Audio::RegisterPlayEndListener(luabridge::LuaRef handler)
{
  m_PlayEndListeners.push_back(handler);
  g_PlayEndListeners.push_back(handler);
}

void Audio::UnregisterPlayEndListener(luabridge::LuaRef handler)
{
  m_PlayEndListeners.erase(std::remove(m_PlayEndListeners.begin(), m_PlayEndListeners.end(), handler), m_PlayEndListeners.end());
  g_PlayEndListeners.erase(std::remove(g_PlayEndListeners.begin(), g_PlayEndListeners.end(), handler), g_PlayEndListeners.end());
}

template <typename T>
inline void removeFromVec(std::vector<T> &A, std::vector<T> &B)
{
  B.erase(std::remove_if(B.begin(), B.end(), [&A](int element)
                         { return std::find(A.begin(), A.end(), element) != A.end(); }),
          B.end());
}

void Audio::Unload()
{
  removeFromVec(m_PlayStartListeners, g_PlayStartListeners);
  removeFromVec(m_PlayEndListeners, g_PlayEndListeners);
  m_PlayStartListeners.clear();
  m_PlayEndListeners.clear();
}

void Audio::OnPlayStart(int slot)
{
  for (auto &listener : g_PlayStartListeners)
  {
    listener(slot);
  }
}

void Audio::OnPlayEnd(int slot)
{
  for (auto &listener : g_PlayEndListeners)
  {
    listener(slot);
  }
}
