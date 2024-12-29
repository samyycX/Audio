#pragma once

#include "serversideclient.h"
#include <string>
#include <vector>
#include <shared_mutex>
#include <networksystem/inetworkmessages.h>
#include <networksystem/inetworkserializer.h>
#include "../protobuf/generated/netmessages.pb.h"

typedef void (*PLAY_START_CALLBACK)(int);
typedef void (*PLAY_END_CALLBACK)(int);

// plugin
extern int FRAMESIZE;
extern int SAMPLERATE;
extern int g_SectionNumber;

struct SVCVoiceDataMessage
{
  std::string voice_data;
  CNetMessagePB<CSVCMsg_VoiceData> *msg = nullptr;
};

extern std::shared_mutex g_Mutex;
extern int MAX_SLOT;

extern std::vector<SVCVoiceDataMessage> g_PlayerAudioBuffer[];
extern std::vector<SVCVoiceDataMessage> g_GlobalAudioBuffer;
extern bool g_PlayerHearing[];
extern float g_PlayerVolume[]; // not implemented yet
extern std::vector<PLAY_START_CALLBACK> g_PlayStartListeners;
extern std::vector<PLAY_END_CALLBACK> g_PlayEndListeners;

extern CServerSideClient *g_AudioPlayerClient;

void InitializeGlobals();
