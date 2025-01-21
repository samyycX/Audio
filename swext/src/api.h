#ifndef _audio_api_h
#define _audio_api_h

#include "iaudioplayer.h"
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
    m_pAudioPlayer = (IAudioPlayer *)g_SMAPI->MetaFactory(AUDIOPLAYER_INTERFACE, nullptr, nullptr);
    g_SMAPI->ConPrintf("%p\n", m_pAudioPlayer);
    if (!g_Initialized)
    {
      g_SMAPI->ConPrintf("123");
      m_pAudioPlayer->RegisterPlayStartListener(&Audio::OnPlayStart);
      g_SMAPI->ConPrintf("123");
      m_pAudioPlayer->RegisterPlayEndListener(&Audio::OnPlayEnd);
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
  void SetPlayerAudioBuffer(int slot, std::string audioBuffer);
  void SetPlayerAudioFile(int slot, std::string audioFile);
  void SetAllAudioBuffer(std::string audioBuffer);
  void SetAllAudioFile(std::string audioFile);
  bool IsPlaying(int slot);
  bool IsAllPlaying();
  void RegisterPlayStartListener(luabridge::LuaRef handler);
  void UnregisterPlayStartListener(luabridge::LuaRef handler);
  void RegisterPlayEndListener(luabridge::LuaRef handler);
  void UnregisterPlayEndListener(luabridge::LuaRef handler);
  void Unload();

protected:
  static void OnPlayStart(int slot);
  static void OnPlayEnd(int slot);

private:
  IAudioPlayer *m_pAudioPlayer;
  std::vector<luabridge::LuaRef> m_PlayStartListeners;
  std::vector<luabridge::LuaRef> m_PlayEndListeners;
};

#endif