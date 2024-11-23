#include "helper.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <stdio.h>
#include "audioplayer.h"
#include "opus/opus.h"
#include "serversideclient.h"
#include <iserver.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#endif

extern INetworkGameServer *g_pNetworkGameServer;
extern IVEngineServer2 *g_pEngineServer2;

CUtlVector<CServerSideClient *> *
GetClientList()
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

std::vector<uint8_t> ConvertAudioBufferToPCM(std::string file_path)
{
  std::vector<uint8_t> outputBuffer;
  std::ostringstream command;
  command << "ffmpeg -y -i \"" << file_path << "\" -acodec pcm_s16le -ac 1 -ar 48000 -f s16le -";

#if defined(_WIN32)
  FILE *pipe = _popen(command.str().c_str(), "rb");
#else
  FILE *pipe = popen(command.str().c_str(), "r");
#endif

  if (!pipe)
  {
    throw std::runtime_error("Failed to run FFmpeg command");
  }

  uint8_t buffer[4096];
  size_t bytesRead;
  while ((bytesRead = fread(buffer, 1, sizeof(buffer), pipe)) > 0)
  {
    outputBuffer.insert(outputBuffer.end(), buffer, buffer + bytesRead);
  }

#if defined(_WIN32)
  _pclose(pipe);
#else
  pclose(pipe);
#endif

  return outputBuffer;
}

std::vector<SVCVoiceDataMessage> FillVoiceMessage(std::vector<std::string> &buffers, CServerSideClient *sender, float factor)
{
  std::vector<SVCVoiceDataMessage> result;
  while (true)
  {
    if (buffers.size() == 0)
      break;
    INetworkMessageInternal *pSVC_VoiceData = g_pNetworkMessages->FindNetworkMessageById(47);
    CNetMessagePB<CSVCMsg_VoiceData> *pData = pSVC_VoiceData->AllocateMessage()->ToPB<CSVCMsg_VoiceData>();
    if (sender)
    {
      pData->set_client(sender->GetPlayerSlot().Get());
    }
    pData->set_xuid(0);
    CMsgVoiceAudio *audio = pData->mutable_audio();
    std::string voice_data;
    int num_packets = 0;
    int packet_offsets = 0;
    for (int i = 0; i < 4; i++)
    {
      if (buffers.size() == 0)
      {
        continue;
      }
      std::string buf = buffers.front();
      voice_data.append(buf);
      audio->add_packet_offsets(packet_offsets + buf.size());
      packet_offsets += buf.size();
      buffers.erase(buffers.begin());
      num_packets += 1;
    }
    g_SectionNumber += 1;
    // audio->set_allocated_voice_data(&voice_data);
    audio->set_format(VoiceDataFormat_t::VOICEDATA_FORMAT_OPUS);
    audio->set_sample_rate(SAMPLERATE);
    audio->set_sequence_bytes(0);
    audio->set_num_packets(num_packets);
    // not sure if this is correct
    audio->set_section_number(g_SectionNumber);
    audio->set_voice_level(factor);
    result.push_back(SVCVoiceDataMessage({voice_data, pData}));
  }

  return result;
}

void ProcessVoiceData(std::string audioBuffer, std::string audioPath, float volumeFactor, std::function<void(std::vector<SVCVoiceDataMessage>)> const &callback)
{
  std::vector<std::string> opus_buffers;
  std::vector<uint8_t> buffer;
  // convert audio to s16be pcm (little endian), 48khz sample rate, 1 channel
  if (audioPath.size() == 0)
  {
    auto timestamp = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    std::string path = g_TempDir + "/audioplayer_" + timestamp + ".tmp";
    std::ofstream outfile(path, std::ios::binary);
    if (!outfile)
    {
      Panic("Can't open file");
    }
    outfile.write(audioBuffer.c_str(), audioBuffer.size());
    outfile.close();
    try
    {
      buffer = ConvertAudioBufferToPCM(path);
    }
    catch (const std::exception &e)
    {
      Panic(e.what());
      return;
    }
    std::filesystem::remove(path);
  }
  else
  {
    try
    {
      buffer = ConvertAudioBufferToPCM(audioPath);
    }
    catch (const std::exception &e)
    {
      Panic(e.what());
      return;
    }
  }

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
      // custom volume is not enabled, all to 1
      long data = ((short)((extracted[i + 1] << 8) | extracted[i])) * volumeFactor;
      if (data < -32768)
      {
        data = -32768;
      }
      if (data > 32767)
      {
        data = 32767;
      }
      pcm_buffer[i / 2] = data & 0xFFFF;
    }

    int opus_size = opus_encode(encoder, pcm_buffer.data(), frame_size / 2,
                                opus_buffer.data(), opus_buffer.size());
    if (opus_size != -1)
    {
      opus_buffer.resize(opus_size);
      std::string data = std::string(opus_buffer.begin(), opus_buffer.end());
      opus_buffers.push_back(data);
    }
  }

  auto msgs = FillVoiceMessage(opus_buffers, nullptr, 0.0f);

  if (callback)
  {
    callback(msgs);
  }
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