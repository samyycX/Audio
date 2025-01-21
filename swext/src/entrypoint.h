#ifndef _entrypoint_h
#define _entrypoint_h

#include <swiftly-ext/core.h>
#include <swiftly-ext/extension.h>

class AudioExtension : SwiftlyExt
{
public:
  bool Load(std::string &error, SourceHook::ISourceHook *SHPtr, ISmmAPI *ismm, bool late);
  bool Unload(std::string &error);

  void AllExtensionsLoaded();
  void AllPluginsLoaded();

  bool OnPluginLoad(std::string pluginName, void *pluginState, PluginKind_t kind, std::string &error);
  bool OnPluginUnload(std::string pluginName, void *pluginState, PluginKind_t kind, std::string &error);

public:
  const char *GetAuthor();
  const char *GetName();
  const char *GetVersion();
  const char *GetWebsite();
};

extern ISmmAPI *g_SMAPI;

DECLARE_GLOBALVARS();

#endif