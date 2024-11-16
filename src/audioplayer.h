#pragma once

#include <ISmmPlugin.h>
#include <serversideclient.h>
#include "../public/iaudioplayer.h"

#ifdef _WIN32
#define ROOTBIN "/bin/win64/"
#define GAMEBIN "/csgo/bin/win64/"
#else
#define ROOTBIN "/bin/linuxsteamrt64/"
#define GAMEBIN "/csgo/bin/linuxsteamrt64/"
#endif

class AudioPlayer : public ISmmPlugin, public IMetamodListener
{
public:
  bool Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late);
  bool Unload(char *error, size_t maxlen);
  void *OnMetamodQuery(const char *iface, int *ret);
  // static void ProcessAudio();

public:
  void Hook_StartupServer(const GameSessionConfiguration_t &config, ISource2WorldSession *, const char *);
  int Hook_LoadEventsFromFile(const char *filename, bool bSearchAll);

public:
  const char *GetAuthor();
  const char *GetName();
  const char *GetDescription();
  const char *GetURL();
  const char *GetLicense();
  const char *GetVersion();
  const char *GetDate();
  const char *GetLogTag();
};

class CAudioPlayerInterface : IAudioPlayer
{
public:
  virtual bool PlayAudio(std::vector<uint8_t> audio_buffer, float voice_level) override;
};