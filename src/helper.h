#include <vector>
#include <iostream>
#include "globals.h"
#include "serversideclient.h"
#include <networksystem/inetworkmessages.h>
#include <networksystem/inetworkserializer.h>
#include "../protobuf/generated/netmessages.pb.h"
#ifdef PLATFORM_LINUX
#include <cstdint>
#endif

CUtlVector<CServerSideClient *> *GetClientList();
CServerSideClient *GetClientBySlot(CPlayerSlot slot);

std::vector<uint8_t> ConvertAudioBufferToPCM(std::string file_path);

SVCVoiceDataMessage FillVoiceMessage(std::vector<std::string> &buffers, CServerSideClient *sender, float voicelevel);

void ProcessVoiceData(int slot, unsigned long id, std::string audioBuffer, std::string audioPath, std::function<void(SVCVoiceDataMessage)> const &callback, float volume);

CServerSideClient *GetFakeClient(const char *name);

void CallPlayStartListeners(int slot);
void CallPlayEndListeners(int slot);
void CallPlayListeners(int slot);