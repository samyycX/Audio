#ifndef _audio_api_h
#define _audio_api_h

#include "iaudio.h"
#include <entrypoint.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <LuaBridge/LuaBridge.h>

#include <functional>

extern bool g_Initialized;
extern std::vector<luabridge::LuaRef> g_PlayStartListeners;
extern std::vector<luabridge::LuaRef> g_PlayEndListeners;

class Audio
{
public:
  Audio()
  {
    m_pAudio = (IAudio *)g_SMAPI->MetaFactory(AUDIO_INTERFACE, nullptr, nullptr);
    if (!g_Initialized)
    {
      m_pAudio->RegisterPlayStartListener(&Audio::OnPlayStart);
      m_pAudio->RegisterPlayEndListener(&Audio::OnPlayEnd);
      g_Initialized = true;
    }
  }
  ~Audio()
  {
    Unload();
  }

  void SetPlayerHearing(int slot, bool hearing);
  void SetAllPlayerHearing(bool hearing);
  bool IsHearing(int slot);
  void PlayToPlayerFromBuffer(int slot, std::string audioBuffer, float volume);
  void PlayToPlayerFromFile(int slot, std::string audioFile, float volume);
  void PlayFromBuffer(std::string audioBuffer, float volume);
  void PlayFromFile(std::string audioFile, float volume);
  bool IsPlaying(int slot);
  bool IsAllPlaying();
  void RegisterPlayStartListener(luabridge::LuaRef handler);
  void UnregisterPlayStartListener(luabridge::LuaRef handler);
  void RegisterPlayEndListener(luabridge::LuaRef handler);
  void UnregisterPlayEndListener(luabridge::LuaRef handler);
  void SetPlayer(int slot);
  void Unload();

protected:
  static void OnPlayStart(int slot);
  static void OnPlayEnd(int slot);

private:
  IAudio *m_pAudio;
  std::vector<luabridge::LuaRef> m_PlayStartListeners;
  std::vector<luabridge::LuaRef> m_PlayEndListeners;
};

#endif