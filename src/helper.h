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

std::vector<SVCVoiceDataMessage> FillVoiceMessage(std::vector<std::string> &buffers, CServerSideClient *sender, float voicelevel);

void ProcessVoiceData(std::string audioBuffer, std::string audioPath, float volumeFactor, std::function<void(std::vector<SVCVoiceDataMessage>)> const &callback);

CServerSideClient *GetFakeClient(const char *name);