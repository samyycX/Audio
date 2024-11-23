#pragma once

#include <vector>
#include <iostream>

#define AUDIOPLAYER_INTERFACE "AudioPlayer002"

class IAudioPlayer
{
public:
  /*
   * @param slot - player slot to set
   * @param hearing - whether player can hear
   */
  virtual void SetPlayerHearing(int slot, bool hearing);
  /*
   * @param hearing - whether player can hear
   */
  virtual void SetAllPlayerHearing(bool hearing);
  /*
   * @param slot - player slot to get
   * @return whether player can hear
   */
  virtual bool IsHearing(int slot) = 0;

  /*
   * @param slot - player slot to set
   * @param audioBuffer - buffer string, contains audio data (like mp3, wav), will be decoded to pcm by ffmpeg,
     pass null and 0 size means stop playing
     @param audioBufferSize - the size of audioBuffer
   */
  virtual void SetPlayerAudioBuffer(int slot, const char *audioBuffer, int audioBufferSize);
  /*
   * @param slot - player slot to set
   * @param audioFile - audio file path, must be absolute path to a audio file (like mp3, wav),
     will be decoded to pcm by ffmpeg
     @param audioFileSize - the size of audioFile
   */
  virtual void SetPlayerAudioFile(int slot, const char *audioFile, int audioFileSize);
  /*
   * @param audioBuffer - buffer string, contains audio data (like mp3, wav), will be decoded to pcm by ffmpeg,
     pass null and 0 size means stop playing
     @param audioBufferSize - the size of audioBuffer
   */
  virtual void SetAllAudioBuffer(const char *audioBuffer, int audioBufferSize);
  /*
   * @param audioFile - audio file path, must be absolute path to a audio file (like mp3, wav),
     will be decoded to pcm by ffmpeg
     @param audioFileSize - the size of audioFile
   */
  virtual void SetAllAudioFile(const char *audioFile, int audioFileSize);
  /*
   * @param slot - player slot to get
   * @return whether there are audio playing for a specific player
   */
  virtual bool IsPlaying(int slot) = 0;
  /*
   * @return whether there are audio playing for all players
   */
  virtual bool IsAllPlaying() = 0;
};
