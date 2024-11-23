#include <string>
#include "../public/iaudioplayer.h"

// not using
// /*
//  * @param slot - player slot to set
//  * @param factor - voice level to set, usually negative
//  */
// void SetPlayerVolume(int slot, float factor);
// void SetAllPlayerVolume(float factor);
// float GetPlayerVolume(int slot);

/*
 * @param slot - player slot to set
 * @param hearing - whether player can hear
 */
void SetPlayerHearing(int slot, bool hearing);
void SetAllPlayerHearing(bool hearing);
bool IsHearing(int slot);

/*
 * @param slot - player slot to set
 * @param voiceData - buffer string, contains pcm data, pass null means stop playing
 * either buffer or path
 */
void SetPlayerAudioBufferString(int slot, std::string audioBuffer, std::string audioPath);
void SetAllAudioBufferString(std::string audioBuffer, std::string audioPath);
bool IsPlaying(int slot);
bool IsAllPlaying();

class CAudioPlayerInterface : IAudioPlayer
{
public:
  void SetPlayerHearing(int slot, bool hearing) override;
  void SetAllPlayerHearing(bool hearing) override;
  bool IsHearing(int slot) override;

  void SetPlayerAudioBuffer(int slot, const char *audioBuffer, int audioBufferSize) override;
  void SetPlayerAudioFile(int slot, const char *audioFile, int audioFileSize) override;
  void SetAllAudioBuffer(const char *audioBuffer, int audioBufferSize) override;
  void SetAllAudioFile(const char *audioFile, int audioFileSize) override;
  bool IsPlaying(int slot) override;
  bool IsAllPlaying() override;
};

extern "C"
{
  __declspec(dllexport) void NativeSetPlayerHearing(int slot, bool hearing);
  __declspec(dllexport) void NativeSetAllPlayerHearing(bool hearing);
  __declspec(dllexport) bool NativeIsHearing(int slot);
  __declspec(dllexport) void NativeSetPlayerAudioBufferString(int slot, const char *audioBuffer, const char *audioPath);
  __declspec(dllexport) void NativeSetAllAudioBufferString(const char *audioBuffer, const char *audioPath);
  __declspec(dllexport) bool NativeIsPlaying(int slot);
  __declspec(dllexport) bool NativeIsAllPlaying();
}