#include "entrypoint.h"
#include "api.h"

#include <string>
#include <sourcehook.h>
#include <ISmmAPI.h>
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
#include <LuaBridge/LuaBridge.h>
#include "swiftly-ext/hooks/FuncHook.h"

AudioExtension g_Ext;
CUtlVector<FuncHookBase *> g_vecHooks;

CREATE_GLOBALVARS();

EXT_EXPOSE(g_Ext);
bool AudioExtension::Load(std::string &error, SourceHook::ISourceHook *SHPtr, ISmmAPI *ismm, bool late)
{
  SAVE_GLOBALVARS();

  return true;
}

bool AudioExtension::OnPluginLoad(std::string pluginName, void *pluginState, PluginKind_t kind, std::string &error)
{
  if (kind != PluginKind_t::Lua)
    return true;
  lua_State *state = (lua_State *)pluginState;
  luabridge::getGlobalNamespace(state)
      .beginClass<Audio>("Audio")
      .addConstructor<void (*)()>()
      .addFunction("SetPlayerHearing", &Audio::SetPlayerHearing)
      .addFunction("SetAllPlayerHearing", &Audio::SetAllPlayerHearing)
      .addFunction("IsHearing", &Audio::IsHearing)
      .addFunction("SetPlayerAudioBuffer", &Audio::SetPlayerAudioBuffer)
      .addFunction("SetPlayerAudioFile", &Audio::SetPlayerAudioFile)
      .addFunction("SetAllAudioBuffer", &Audio::SetAllAudioBuffer)
      .addFunction("SetAllAudioFile", &Audio::SetAllAudioFile)
      .addFunction("IsPlaying", &Audio::IsPlaying)
      .addFunction("IsAllPlaying", &Audio::IsAllPlaying)
      .addFunction("RegisterPlayStartListener", &Audio::RegisterPlayStartListener)
      .addFunction("UnregisterPlayStartListener", &Audio::UnregisterPlayStartListener)
      .addFunction("RegisterPlayEndListener", &Audio::RegisterPlayEndListener)
      .addFunction("UnregisterPlayEndListener", &Audio::UnregisterPlayEndListener)
      .addFunction("Unload", &Audio::Unload)
      .endClass();
  return true;
}
bool AudioExtension::Unload(std::string &error)
{
  return true;
}
bool AudioExtension::OnPluginUnload(std::string pluginName, void *pluginState, PluginKind_t kind, std::string &error)
{
  return true;
}
void AudioExtension::AllExtensionsLoaded()
{
}

void AudioExtension::AllPluginsLoaded()
{
}

const char *AudioExtension::GetAuthor()
{
  return "samyyc";
}

const char *AudioExtension::GetName()
{
  return "Audio";
}

const char *AudioExtension::GetVersion()
{
  return "1.0.0";
}

const char *AudioExtension::GetWebsite()
{
  return "https://github.com/samyycX/AudioPlayer";
}