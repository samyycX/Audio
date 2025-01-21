#include "api.h"

bool g_Initialized = false;
std::vector<luabridge::LuaRef> g_PlayStartListeners;
std::vector<luabridge::LuaRef> g_PlayEndListeners;

void Audio::SetPlayerHearing(int slot, bool hearing)
{
  m_pAudio->SetPlayerHearing(slot, hearing);
}

void Audio::SetAllPlayerHearing(bool hearing)
{
  m_pAudio->SetAllPlayerHearing(hearing);
}

bool Audio::IsHearing(int slot)
{
  return m_pAudio->IsHearing(slot);
}

void Audio::PlayToPlayerFromBuffer(int slot, std::string audioBuffer, float volume)
{
  m_pAudio->PlayToPlayerFromBuffer(slot, audioBuffer, volume);
}

void Audio::PlayToPlayerFromFile(int slot, std::string audioFile, float volume)
{
  m_pAudio->PlayToPlayerFromFile(slot, audioFile, volume);
}

void Audio::PlayFromBuffer(std::string audioBuffer, float volume)
{
  m_pAudio->PlayFromBuffer(audioBuffer, volume);
}

void Audio::PlayFromFile(std::string audioFile, float volume)
{
  m_pAudio->PlayFromFile(audioFile, volume);
}

bool Audio::IsPlaying(int slot)
{
  return m_pAudio->IsPlaying(slot);
}

bool Audio::IsAllPlaying()
{
  return m_pAudio->IsAllPlaying();
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
void Audio::SetPlayer(int slot)
{
  m_pAudio->SetPlayer(slot);
}

template <typename T>
inline void removeFromVec(std::vector<T> &A, std::vector<T> &B)
{
  for (auto &element : A)
  {
    B.erase(std::remove(B.begin(), B.end(), element), B.end());
  }
}

void Audio::Unload()
{
  removeFromVec(m_PlayStartListeners, g_PlayStartListeners);
  removeFromVec(m_PlayEndListeners, g_PlayEndListeners);
  // m_PlayStartListeners.clear();
  // m_PlayEndListeners.clear();
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
