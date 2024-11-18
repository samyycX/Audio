#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <funchook.h>
#include "audioplayer.h"
#include "utils/module.h"
#include <iserver.h>
#include <ISmmAPI.h>
#include <serversideclient.h>
#include <strtools.h>
#include <playerslot.h>
#include <eiface.h>
#include <entitysystem.h>
#include <igameevents.h>
#include "engine/igameeventsystem.h"
#include "eventlistener.h"
#include "opus/opus.h"
#include "tier1/strtools.h"
#include "audiocode.h"
#include <networksystem/inetworkmessages.h>
#include <networksystem/inetworkserializer.h>
#include "../protobuf/generated/netmessages.pb.h"

// typedef char(FASTCALL *SV_BroadcastVoiceData_t)(int64 idk, CServerSideClient *client, CMsgVoiceAudio *data, int64 xuid);
// typedef CMsgVoiceAudio *(FASTCALL *CMSgVoiceAudio_Constructor_t)(int64 a);

AudioPlayer g_AudioPlayer;
INetworkGameServer *g_pNetworkGameServer = nullptr;
IVEngineServer2 *g_pEngineServer2 = nullptr;
IGameEventSystem *g_gameEventSystem = nullptr;
IGameEventManager2 *g_gameEventManager = nullptr;
int g_iLoadEventsFromFileId;

CModule *engine = nullptr;
CModule *server = nullptr;

// SV_BroadcastVoiceData_t g_pfnSVBroadcastVoiceData = nullptr;
// CMSgVoiceAudio_Constructor_t g_pfnCMsgVoiceAudioConstructor = nullptr;

OpusEncoder *encoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_AUDIO, NULL);
const int FRAMESIZE = 960;
const int SAMPLERATE = 48000;
int g_SectionNumber = 0;

CServerSideClient *g_AudioPlayerClient = nullptr;

bool g_bPlaying = false;

CAudioPlayerInterface g_AudioPlayerInterface;

PLUGIN_EXPOSE(AudioPlayer, g_AudioPlayer);

class GameSessionConfiguration_t
{
};

SH_DECL_HOOK3_void(INetworkServerService, StartupServer, SH_NOATTRIB, 0, const GameSessionConfiguration_t &, ISource2WorldSession *, const char *);
SH_DECL_HOOK2(IGameEventManager2, LoadEventsFromFile, SH_NOATTRIB, 0, int, const char *, bool);
void Message(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);

    char buf[1024] = {};
    V_vsnprintf(buf, sizeof(buf) - 1, msg, args);

    ConColorMsg(Color(0, 255, 200), "[AUDIOPLAYER] %s", buf);

    va_end(args);
}

void Panic(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);

    char buf[1024] = {};
    V_vsnprintf(buf, sizeof(buf) - 1, msg, args);

    Warning("[AUDIOPLAYER] %s", buf);

    va_end(args);
}

// CMsgVoiceAudio *NewCMsgVoiceAudio()
// {
//     return g_pfnCMsgVoiceAudioConstructor(0);
// }
CUtlVector<CServerSideClient *> *GetClientList()
{
    if (!g_pNetworkGameServer)
        return nullptr;

#ifdef PLATFORM_WINDOWS
    static constexpr int offset = 78;
#else
    static constexpr int offset = 80;
#endif
    return (CUtlVector<CServerSideClient *> *)(&g_pNetworkGameServer[offset]);
}

CServerSideClient *GetClientBySlot(CPlayerSlot slot)
{
    CUtlVector<CServerSideClient *> *pClients = GetClientList();

    if (!pClients)
        return nullptr;

    return pClients->Element(slot.Get());
}

CServerSideClient *GetFakeClient(const char *name)
{
    auto pClients = GetClientList();
    CServerSideClient *fakeClient = nullptr;
    for (int i = 0; i < pClients->Count(); i++)
    {
        if (pClients->IsValidIndex(i))
        {
            auto client = pClients->Element(i);
            if (client->IsInGame() && (client->IsFakePlayer() || client->IsHLTV()))
            {
                fakeClient = client;
                break;
            }
        }
    }
    if (!fakeClient)
    {
        CPlayerSlot slot = g_pEngineServer2->CreateFakeClient(name);
        fakeClient = GetClientBySlot(slot);
    }
    return fakeClient;
}

void ProcessAudio(std::string filename, float voice_level)
{
    g_bPlaying = true;
    // convert audio to s16be pcm (big endian), 48khz sample rate, 1 channel
    std::vector<uint8_t> buffer;
    try
    {
        buffer = convertAudioBufferToPCM(filename);
    }
    catch (const std::exception &e)
    {
        g_bPlaying = false;
        Panic(e.what());
        return;
    }

    // std::chrono::steady_clock::time_point last_pull;
    struct OpusBuffer
    {
        std::string data;
        int opus_size;
    };
    std::vector<OpusBuffer> opus_buffers;

    // cs2 opus: 48khz samplerate, 480 framesize, a message can hold 4 packets, so we need to start a thread to send it each 40ms
    Message("Encoding to opus...\n");
    while (true)
    {
        if (buffer.size() == 0)
            break;
        std::vector<int16_t> pcm_buffer(FRAMESIZE * 1 * 2);
        std::vector<unsigned char> opus_buffer(2048);
        auto frame_size = std::min<size_t>(FRAMESIZE, buffer.size());
        std::vector<uint8_t> extracted(buffer.begin(), buffer.begin() + frame_size);
        buffer.erase(buffer.begin(), buffer.begin() + frame_size);

        for (int i = 0; i < extracted.size(); i += 2)
        {
            pcm_buffer[i / 2] = (extracted[i] << 8) | extracted[i + 1];
        }

        int opus_size = opus_encode(encoder, pcm_buffer.data(), frame_size / 2,
                                    opus_buffer.data(), opus_buffer.size());
        if (opus_size != -1)
        {
            opus_buffer.resize(opus_size);
            std::string data = std::string(opus_buffer.begin(), opus_buffer.end());
            opus_buffers.push_back({data, opus_size});
        }
    }

    Message("Start playing...\n");
    while (true)
    {
        if (opus_buffers.size() == 0)
        {
            break;
        }
        g_SectionNumber += 1;
        INetworkMessageInternal *pSVC_VoiceData = g_pNetworkMessages->FindNetworkMessageById(47);
        CNetMessagePB<CSVCMsg_VoiceData> *pData = pSVC_VoiceData->AllocateMessage()->ToPB<CSVCMsg_VoiceData>();
        pData->set_client(g_AudioPlayerClient->GetPlayerSlot().Get());
        pData->set_xuid(0);
        CMsgVoiceAudio *audio = pData->mutable_audio();
        std::string voice_data;
        int num_packets = 0;
        int packet_offsets = 0;
        for (int i = 0; i < 4; i++)
        {
            if (opus_buffers.size() == 0)
            {
                continue;
            }
            OpusBuffer buf = opus_buffers.front();
            voice_data.append(buf.data);
            audio->add_packet_offsets(packet_offsets + buf.opus_size);
            packet_offsets += buf.opus_size;
            opus_buffers.erase(opus_buffers.begin());
            num_packets += 1;
        }
        audio->set_allocated_voice_data(&voice_data);
        audio->set_format(VoiceDataFormat_t::VOICEDATA_FORMAT_OPUS);
        audio->set_sample_rate(SAMPLERATE);
        audio->set_sequence_bytes(0);
        audio->set_num_packets(num_packets);
        // not sure if this is correct
        audio->set_section_number(g_SectionNumber);
        audio->set_voice_level(voice_level);
        CUtlVector<CServerSideClient *> *client_list = GetClientList();
        for (int i = 0; i < client_list->Count(); i++)
        {
            if (!client_list->IsValidIndex(i))
            {
                continue;
            }
            CServerSideClient *client = client_list->Element(i);
            if (client->IsInGame() && !client->IsFakePlayer() && !client->IsHLTV())
            {
                client->GetNetChannel()->SendNetMessage(pData, NetChannelBufType_t::BUF_VOICE);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(39));
    }
    g_bPlaying = false;
    return;
}
bool AudioPlayer::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
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
    SH_ADD_HOOK(INetworkServerService, StartupServer, g_pNetworkServerService, SH_MEMBER(this, &AudioPlayer::Hook_StartupServer), true);
    auto pCGameEventManagerVTable = (IGameEventManager2 *)server->FindVirtualTable("CGameEventManager");
    g_iLoadEventsFromFileId = SH_ADD_DVPHOOK(IGameEventManager2, LoadEventsFromFile, pCGameEventManagerVTable, SH_MEMBER(this, &AudioPlayer::Hook_LoadEventsFromFile), false);
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
    return true;
}

bool AudioPlayer::Unload(char *error, size_t maxlen)
{
    SH_REMOVE_HOOK(INetworkServerService, StartupServer, g_pNetworkServerService, SH_MEMBER(this, &AudioPlayer::Hook_StartupServer), true);
    SH_REMOVE_HOOK_ID(g_iLoadEventsFromFileId);

    UnregisterEventListeners();

    if (g_pNetworkGameServer)
        delete g_pNetworkGameServer;
    if (g_pEngineServer2)
        delete g_pEngineServer2;
    if (encoder)
        delete encoder;

    return true;
}

void *AudioPlayer::OnMetamodQuery(const char *iface, int *ret)
{
    if (V_strcmp(iface, AUDIOPLAYER_INTERFACE))
    {
        if (ret)
            *ret = META_IFACE_FAILED;

        return nullptr;
    }

    if (ret)
        *ret = META_IFACE_OK;

    return &g_AudioPlayerInterface;
}

void AudioPlayer::Hook_StartupServer(const GameSessionConfiguration_t &config, ISource2WorldSession *session, const char *mapname)
{
    g_pNetworkGameServer = g_pNetworkServerService->GetIGameServer();
    RegisterEventListeners();
}

int AudioPlayer::Hook_LoadEventsFromFile(const char *filename, bool bSearchAll)
{
    ExecuteOnce(g_gameEventManager = META_IFACEPTR(IGameEventManager2));

    RETURN_META_VALUE(MRES_IGNORED, 0);
}

bool CAudioPlayerInterface::PlayAudio(std::string filename, float voice_level)
{
    if (g_bPlaying)
    {
        Panic("Already playing!");
        return 0;
    }
    g_AudioPlayerClient = GetFakeClient("[AUDIOPLAYER] Player");
    std::thread processThread(ProcessAudio, filename, voice_level);
    processThread.detach();
    return 1;
}

const char *AudioPlayer::GetLicense()
{
    return "GPL v3 License";
}

const char *AudioPlayer::GetVersion()
{
    return "1.1.0";
}

const char *AudioPlayer::GetDate()
{
    return __DATE__;
}

const char *AudioPlayer::GetLogTag()
{
    return "AudioPlayer";
}

const char *AudioPlayer::GetAuthor()
{
    return "samyyc";
}

const char *AudioPlayer::GetDescription()
{
    return "A voice player to play custom audio.";
}

const char *AudioPlayer::GetName()
{
    return "AudioPlayer";
}

const char *AudioPlayer::GetURL()
{
    return "https://github.com/samyycX/AudioPlayer";
}