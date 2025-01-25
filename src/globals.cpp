#include "globals.h"
#include "serversideclient.h"
#include <string.h>

int FRAMESIZE = 960;
int SAMPLERATE = 48000;
int g_SectionNumber = 0;

std::shared_mutex g_Mutex;
const int _MAX_SLOT = 64;
int MAX_SLOT = _MAX_SLOT;

std::vector<SVCVoiceDataMessage> g_PlayerAudioBuffer[_MAX_SLOT] = {};
int g_PlayerProgress[_MAX_SLOT];
std::vector<SVCVoiceDataMessage> g_GlobalAudioBuffer;
int g_GlobalProgress = 0;
bool g_PlayerHearing[_MAX_SLOT];
float g_PlayerVolume[_MAX_SLOT];
PLAY_START_CALLBACK g_PlayStartListeners[MAX_LISTENERS] = {nullptr};
PLAY_END_CALLBACK g_PlayEndListeners[MAX_LISTENERS] = {nullptr};
PLAY_CALLBACK g_PlayListeners[MAX_LISTENERS] = {nullptr};

CServerSideClient *g_AudioPlayerClient;

int g_Player = -1;

void InitializeGlobals()
{
  for (int i = 0; i < MAX_SLOT; i++)
  {
    g_PlayerHearing[i] = true;
    g_PlayerVolume[i] = 1.0f;
    g_PlayerProgress[i] = 0;
  }
}