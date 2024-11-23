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

#ifdef PLATFORM_LINUX
#define PINVOKE_EXPORT __attribute__((visibility("default")))
#else
#define PINVOKE_EXPORT __declspec(dllexport)
#endif

extern "C"
{
  PINVOKE_EXPORT void NativeSetPlayerHearing(int slot, bool hearing);
  PINVOKE_EXPORT void NativeSetAllPlayerHearing(bool hearing);
  PINVOKE_EXPORT bool NativeIsHearing(int slot);
  PINVOKE_EXPORT void NativeSetPlayerAudioBufferString(int slot, const char *audioBuffer, int audioBufferSize, const char *audioPath, int audioPathSize);
  PINVOKE_EXPORT void NativeSetAllAudioBufferString(const char *audioBuffer, int audioBufferSize, const char *audioPath, int audioPathSize);
  PINVOKE_EXPORT bool NativeIsPlaying(int slot);
  PINVOKE_EXPORT bool NativeIsAllPlaying();
}