#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <funchook.h>
#include "audio.h"
#include "utils/module.h"
#include <iserver.h>
#include <ISmmAPI.h>
#include <serversideclient.h>
#include <strtools.h>
#include <playerslot.h>
#include <eiface.h>
#include <entitysystem.h>
#include <igameevents.h>
#include <filesystem>
#include "engine/igameeventsystem.h"
#include "eventlistener.h"
#include "opus/opus.h"
#include "tier1/strtools.h"
#include "helper.h"
#include "globals.h"
#include "audioapi.h"
#include <networksystem/inetworkmessages.h>
#include <networksystem/inetworkserializer.h>
#include "../protobuf/generated/netmessages.pb.h"

// typedef char(FASTCALL *SV_BroadcastVoiceData_t)(int64 idk, CServerSideClient *client, CMsgVoiceAudio *data, int64 xuid);
// typedef CMsgVoiceAudio *(FASTCALL *CMSgVoiceAudio_Constructor_t)(int64 a);

Audio g_Audio;
INetworkGameServer *g_pNetworkGameServer = nullptr;
IVEngineServer2 *g_pEngineServer2 = nullptr;
IGameEventSystem *g_gameEventSystem = nullptr;
IGameEventManager2 *g_gameEventManager = nullptr;
int g_iLoadEventsFromFileId;
std::string g_TempDir;
bool initialized = false;

CModule *engine = nullptr;
CModule *server = nullptr;

OpusEncoder *encoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_AUDIO, NULL);

bool g_bPlaying = false;

CAudioInterface g_AudioInterface;

std::thread VoiceDataSendingThread;

PLUGIN_EXPOSE(Audio, g_Audio);

class GameSessionConfiguration_t
{
};

SH_DECL_HOOK3_void(IServerGameDLL, GameFrame, SH_NOATTRIB, 0, bool, bool, bool);
SH_DECL_HOOK3_void(INetworkServerService, StartupServer, SH_NOATTRIB, 0, const GameSessionConfiguration_t &, ISource2WorldSession *, const char *);
SH_DECL_HOOK2(IGameEventManager2, LoadEventsFromFile, SH_NOATTRIB, 0, int, const char *, bool);
void Message(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);

    char buf[1024] = {};
    V_vsnprintf(buf, sizeof(buf) - 1, msg, args);

    ConColorMsg(Color(0, 255, 200), "[Audio] %s", buf);

    va_end(args);
}

void Panic(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);

    char buf[1024] = {};
    V_vsnprintf(buf, sizeof(buf) - 1, msg, args);

    Warning("[Audio] %s", buf);

    va_end(args);
}
void SendVoiceDataLoop()
{
    // std::chrono::steady_clock::time_point last_pull;

    // cs2 opus: 48khz samplerate, 480 framesize, a message can hold 4 packets, so we need to start a thread to send it each 40ms
    while (true)
    {
        int interval = 29000000 - g_LastDelay;

        // int interval = 30000000 - g_LastDelay;
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        if (!g_bPlaying)
        {
            std::this_thread::sleep_for(std::chrono::nanoseconds(interval));
            continue;
        }
        CUtlVector<CServerSideClient *> *client_list = GetClientList();
        if (client_list->Count() == 0)
        {
            std::this_thread::sleep_for(std::chrono::nanoseconds(interval));
            continue;
        }
        SVCVoiceDataMessage all_data;
        bool played = false;
        if (!g_GlobalAudioBuffer.empty())

        {
            all_data = g_GlobalAudioBuffer.front();
            g_GlobalAudioBuffer.erase(g_GlobalAudioBuffer.begin());
            played = true;
            if (g_GlobalAudioBuffer.empty() && g_ProcessingThreads.Find(-1) == g_ProcessingThreads.InvalidIndex())
            {
                // call all play end listeners
                for (auto &callback : g_PlayEndListeners)
                {
                    if (callback != nullptr)
                        callback(-1);
                }
            }
            else
            {
                for (auto &callback : g_PlayListeners)
                {
                    if (callback != nullptr)
                        callback(-1);
                }
            }
        }
        CNetMessagePB<CSVCMsg_VoiceData> *pAllData = nullptr;
        if (all_data.msg)
        {
            INetworkMessageInternal *pSVC_VoiceData = g_pNetworkMessages->FindNetworkMessageById(47);
            pAllData = pSVC_VoiceData->AllocateMessage()->ToPB<CSVCMsg_VoiceData>();
            pAllData->CopyFrom(*all_data.msg);
            // pAllData->set_client(g_Player);
            pAllData->set_client(0xFFFFFFF7);
            std::string *copied_all_data = new std::string(all_data.voice_data);
            pAllData->mutable_audio()->set_allocated_voice_data(copied_all_data);
            pAllData->mutable_audio()->set_section_number(g_SectionNumber);
        }

        for (int i = 0; i < client_list->Count(); i++)
        {
            if (!client_list->IsValidIndex(i))
                continue;
            SVCVoiceDataMessage player_data;
            CServerSideClient *client = client_list->Element(i);
            if (!client->IsInGame() || client->IsFakePlayer() || client->IsHLTV())
                continue;
            int slot = client->GetPlayerSlot().Get();
            std::vector<SVCVoiceDataMessage> *playerBuffer = &g_PlayerAudioBuffer[slot];
            if (!playerBuffer->empty())
            {
                player_data = playerBuffer->front();
                playerBuffer->erase(playerBuffer->begin());
                played = true;
                if (playerBuffer->empty() && g_ProcessingThreads.Find(slot) == g_ProcessingThreads.InvalidIndex())
                {
                    // call all play end listeners
                    for (auto &callback : g_PlayEndListeners)
                    {
                        if (callback != nullptr)
                            callback(slot);
                    }
                }
                else
                {
                    for (auto &callback : g_PlayListeners)
                    {
                        if (callback != nullptr)
                            callback(slot);
                    }
                }
            }
            if (!all_data.msg && !player_data.msg)
            {
                continue;
            }

            if (!api::IsHearing(slot))
                continue;
            if (player_data.msg)
            {
                INetworkMessageInternal *pSVC_VoiceData = g_pNetworkMessages->FindNetworkMessageById(47);
                CNetMessagePB<CSVCMsg_VoiceData> *pData = pSVC_VoiceData->AllocateMessage()->ToPB<CSVCMsg_VoiceData>();
                pData->CopyFrom(*player_data.msg);
                pData->set_client(g_Player);
                std::string *copied_data = new std::string(player_data.voice_data);
                pData->mutable_audio()->set_allocated_voice_data(copied_data);
                pData->mutable_audio()->set_section_number(g_SectionNumber);
                g_QueuedNextFrameFunc.push_back([client, pData]()
                                                { client->GetNetChannel()->SendNetMessage(pData, NetChannelBufType_t::BUF_VOICE); });
                // client->GetNetChannel()->SendNetMessage(pData, NetChannelBufType_t::BUF_VOICE);
            }
            else if (pAllData)
            {
                g_QueuedNextFrameFunc.push_back([client, pAllData]()
                                                { client->GetNetChannel()->SendNetMessage(pAllData, NetChannelBufType_t::BUF_VOICE); });
                // client->GetNetChannel()->SendNetMessage(pAllData, NetChannelBufType_t::BUF_VOICE);
            }

            // data.msg->mutable_audio()->set_voice_level(GetPlayerVolume(slot));

            // my test:
            // real player -> play from real player
            // fake client -> play from a bot which has no team, need sv_alltalk 1
            // 1 (non-exist but legit client index) -> a skeleton icon with no name playing the audio, no need sv_alltalk 1
            // 1337 (non-exist and illegal client index) -> no display, but still playing audio, no need sv_alltalk 1
            // btw, calling CreateFakeClient in this thread will cause weird bug in counterstrikesharp
            // if (player_data.msg)
            // {
            //     player_data.Destroy();
            // }
        }
        // if (all_data.msg)
        // {
        //     all_data.Destroy();
        // }
        if (played)
        {
            g_SectionNumber += 1;
        }
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::nano> elapsed = end - start;
        g_LastDelay = elapsed.count();
        if (g_LastDelay > g_MaxDelay)
        {
            // write to file
            std::string path = g_TempDir + "/max_delay.txt";
            std::ofstream file(path, std::ios::app);
            file << g_LastDelay << std::endl;
            file.close();
            g_MaxDelay = g_LastDelay;

            Message("Max delay exceeded: %f milliseconds\n", g_MaxDelay / 1000000.0);
        }

        // Message("Elapsed time: %f milliseconds\n", elapsed.count());
        std::this_thread::sleep_for(std::chrono::nanoseconds(interval));
    }
}

bool Audio::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
    PLUGIN_SAVEVARS();

    GET_V_IFACE_CURRENT(GetEngineFactory, g_pEngineServer2, IVEngineServer2, SOURCE2ENGINETOSERVER_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pGameResourceServiceServer, IGameResourceService, GAMERESOURCESERVICESERVER_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetEngineFactory, g_gameEventSystem, IGameEventSystem, GAMEEVENTSYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetEngineFactory, g_pNetworkServerService, INetworkServerService, NETWORKSERVERSERVICE_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetServerFactory, g_pSource2GameClients, IServerGameClients, SOURCE2GAMECLIENTS_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetServerFactory, g_pSource2Server, ISource2Server, SOURCE2SERVER_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetEngineFactory, g_pNetworkMessages, INetworkMessages, NETWORKMESSAGES_INTERFACE_VERSION);

    g_SMAPI->AddListener(this, this);

    engine = new CModule(ROOTBIN, "engine2");
    server = new CModule(GAMEBIN, "server");
    SH_ADD_HOOK(INetworkServerService, StartupServer, g_pNetworkServerService, SH_MEMBER(this, &Audio::Hook_StartupServer), true);
    SH_ADD_HOOK(IServerGameDLL, GameFrame, g_pSource2Server, SH_MEMBER(this, &Audio::Hook_GameFramePre), false);
    auto pCGameEventManagerVTable = (IGameEventManager2 *)server->FindVirtualTable("CGameEventManager");
    g_iLoadEventsFromFileId = SH_ADD_DVPHOOK(IGameEventManager2, LoadEventsFromFile, pCGameEventManagerVTable, SH_MEMBER(this, &Audio::Hook_LoadEventsFromFile), false);
    if (late)
    {
        RegisterEventListeners();
        g_pNetworkGameServer = g_pNetworkServerService->GetIGameServer();
    }
    // #ifdef PLATFORM_WINDOWS
    //     const byte SV_BroadcastVoiceData_Sig[] = "\x48\x89\x74\x24\x20\x48\x89\x54\x24\x10\x55\x57\x41\x55";
    //     const byte CMsgVoiceAudio_Constructor_Sig[] = "\x48\x89\x5C\x24\x10\x48\x89\x74\x24\x18\x57\x48\x83\xEC\x20\x33\xF6\x48\x8B\xF9\x8D\x56\x58";
    // #else
    //     const byte SV_BroadcastVoiceData_Sig[] = "\x55\x48\x89\xE5\x41\x57\x49\x89\xD7\x41\x56\x49\x89\xF6\xBE\xFF\xFF\xFF\xFF";
    //     const byte CMsgVoiceAudio_Constructor_Sig[] = "\x55\x48\x89\xE5\x53\x48\x83\xEC\x08\x48\x85\xFF\x74\x2A\x48\x8D\x15\x73\xA6\x5D\x00";
    // #endif
    //     int sig_error;
    //     g_pfnSVBroadcastVoiceData = (SV_BroadcastVoiceData_t)engine->FindSignature(SV_BroadcastVoiceData_Sig, sizeof(SV_BroadcastVoiceData_Sig) - 1, sig_error);
    //     g_pfnCMsgVoiceAudioConstructor = (CMSgVoiceAudio_Constructor_t)engine->FindSignature(CMsgVoiceAudio_Constructor_Sig, sizeof(CMsgVoiceAudio_Constructor_Sig) - 1, sig_error);

    char tempDir[512];
    ismm->Format(tempDir, sizeof(tempDir), "%s/addons/audio/temp", ismm->GetBaseDir());
    std::filesystem::create_directory(tempDir);
    g_TempDir = std::string(tempDir);
    for (char &ch : g_TempDir)
    {
        if (ch == '\\')
        {
            ch = '/';
        }
    }
    InitializeGlobals();
    return true;
}

bool Audio::Unload(char *error, size_t maxlen)
{
    SH_REMOVE_HOOK(INetworkServerService, StartupServer, g_pNetworkServerService, SH_MEMBER(this, &Audio::Hook_StartupServer), true);
    SH_REMOVE_HOOK_ID(g_iLoadEventsFromFileId);

    UnregisterEventListeners();

    if (g_pNetworkGameServer)
        delete g_pNetworkGameServer;
    if (g_pEngineServer2)
        delete g_pEngineServer2;
    if (encoder)
        delete encoder;

    initialized = false;
    g_bPlaying = false;
    return true;
}

void *Audio::OnMetamodQuery(const char *iface, int *ret)
{
    if (V_strcmp(iface, AUDIO_INTERFACE))
    {
        if (ret)
            *ret = META_IFACE_FAILED;

        return nullptr;
    }

    if (ret)
        *ret = META_IFACE_OK;

    return &g_AudioInterface;
}

void Audio::Hook_StartupServer(const GameSessionConfiguration_t &config, ISource2WorldSession *session, const char *mapname)
{
    g_pNetworkGameServer = g_pNetworkServerService->GetIGameServer();
    RegisterEventListeners();
    if (!initialized)
    {
        VoiceDataSendingThread = std::thread(SendVoiceDataLoop);
        VoiceDataSendingThread.detach();
        initialized = true;
    }
    // g_bPlaying = 1;
}

void Audio::OnLevelShutdown()
{
    Message("[Audio] The loop is paused here.");
    g_bPlaying = false;
    
    if(!g_GlobalAudioBuffer.empty())
        g_GlobalAudioBuffer.clear();
}

void Audio::Hook_GameFramePre(bool simulating, bool bFirstTick, bool bLastTick)
{
    /**
     * simulating:
     * ***********
     * true  | game is ticking
     * false | game is not ticking
     */

    if (!simulating)
        return;
    for (auto &func : g_QueuedNextFrameFunc)
    {
        func();
    }
    g_QueuedNextFrameFunc.clear();
}

int Audio::Hook_LoadEventsFromFile(const char *filename, bool bSearchAll)
{
    ExecuteOnce(g_gameEventManager = META_IFACEPTR(IGameEventManager2));

    RETURN_META_VALUE(MRES_IGNORED, 0);
}

const char *Audio::GetLicense()
{
    return "GPL v3 License";
}

const char *Audio::GetVersion()
{
    return "2.0.1";
}

const char *Audio::GetDate()
{
    return __DATE__;
}

const char *Audio::GetLogTag()
{
    return "Audio";
}

const char *Audio::GetAuthor()
{
    return "samyyc";
}

const char *Audio::GetDescription()
{
    return "A voice player to play custom audio.";
}

const char *Audio::GetName()
{
    return "Audio";
}

const char *Audio::GetURL()
{
    return "https://github.com/samyycX/Audio";
}