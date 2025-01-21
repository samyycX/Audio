#include <string>
#include "../public/iaudio.h"
#include <globals.h>

// not using
// /*
//  * @param slot - player slot to set
//  * @param factor - voice level to set, usually negative
//  */
// void SetPlayerVolume(int slot, float factor);
// void SetAllPlayerVolume(float factor);
// float GetPlayerVolume(int slot);

namespace api
{
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
  void PlayToPlayer(int slot, std::string audioBuffer, std::string audioPath, float volume);
  void Play(std::string audioBuffer, std::string audioPath, float volume);
  bool IsPlaying(int slot);
  bool IsAllPlaying();

  int RegisterPlayStartListener(PLAY_START_CALLBACK callback);
  void UnregisterPlayStartListener(int id);
  int RegisterPlayEndListener(PLAY_END_CALLBACK callback);
  void UnregisterPlayEndListener(int id);

}

class CAudioInterface : IAudio
{

public:
  virtual void SetPlayerHearing(int slot, bool hearing) override;
  virtual void SetAllPlayerHearing(bool hearing) override;
  virtual bool IsHearing(int slot) override;

  virtual void PlayToPlayerFromBuffer(int slot, const char *audioBuffer, int audioBufferSize, float volume = 1.0) override;
  virtual void PlayToPlayerFromFile(int slot, const char *audioFile, int audioFileSize, float volume = 1.0) override;
  virtual void PlayFromBuffer(const char *audioBuffer, int audioBufferSize, float volume = 1.0) override;
  virtual void PlayFromFile(const char *audioFile, int audioFileSize, float volume = 1.0) override;
  virtual bool IsPlaying(int slot) override;
  virtual bool IsAllPlaying() override;

  virtual int RegisterPlayStartListener(PLAY_START_CALLBACK callback);
  virtual void UnregisterPlayStartListener(int id);
  virtual int RegisterPlayEndListener(PLAY_END_CALLBACK callback);
  virtual void UnregisterPlayEndListener(int id);
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

  PINVOKE_EXPORT void NativePlayToPlayer(int slot, const char *audioBuffer, int audioBufferSize, const char *audioPath, int audioPathSize, float volume = 1.0);
  PINVOKE_EXPORT void NativePlay(const char *audioBuffer, int audioBufferSize, const char *audioPath, int audioPathSize, float volume = 1.0);
  PINVOKE_EXPORT bool NativeIsPlaying(int slot);
  PINVOKE_EXPORT bool NativeIsAllPlaying();

  PINVOKE_EXPORT int NativeRegisterPlayStartListener(PLAY_START_CALLBACK callback);
  PINVOKE_EXPORT void NativeUnregisterPlayStartListener(int id);
  PINVOKE_EXPORT int NativeRegisterPlayEndListener(PLAY_END_CALLBACK callback);
  PINVOKE_EXPORT void NativeUnregisterPlayEndListener(int id);
}