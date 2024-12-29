#pragma once

#include <vector>
#include <iostream>

#define AUDIOPLAYER_INTERFACE "AudioPlayer002"

class IAudioPlayer
{

  typedef void (*PLAY_START_HANDLER)(int);
  typedef void (*PLAY_END_HANDLER)(int);

public:
  /*
   * @param slot - player slot to set
   * @param hearing - whether player can hear
   */
  virtual void SetPlayerHearing(int slot, bool hearing) = 0;
  /*
   * @param hearing - whether player can hear
   */
  virtual void SetAllPlayerHearing(bool hearing) = 0;
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
  virtual void SetPlayerAudioBuffer(int slot, const char *audioBuffer, int audioBufferSize) = 0;
  /*
   * @param slot - player slot to set
   * @param audioFile - audio file path, must be absolute path to a audio file (like mp3, wav),
     will be decoded to pcm by ffmpeg
     @param audioFileSize - the size of audioFile
   */
  virtual void SetPlayerAudioFile(int slot, const char *audioFile, int audioFileSize) = 0;
  /*
   * @param audioBuffer - buffer string, contains audio data (like mp3, wav), will be decoded to pcm by ffmpeg,
     pass null and 0 size means stop playing
     @param audioBufferSize - the size of audioBuffer
   */
  virtual void SetAllAudioBuffer(const char *audioBuffer, int audioBufferSize) = 0;
  /*
   * @param audioFile - audio file path, must be absolute path to a audio file (like mp3, wav),
     will be decoded to pcm by ffmpeg
     @param audioFileSize - the size of audioFile
   */
  virtual void SetAllAudioFile(const char *audioFile, int audioFileSize) = 0;
  /*
   * @param slot - player slot to get
   * @return whether there are audio playing for a specific player
   */
  virtual bool IsPlaying(int slot) = 0;
  /*
   * @return whether there are audio playing for all players
   */
  virtual bool IsAllPlaying() = 0;
  /*
   * @param handler - play start handler
   * @return id - listener id, you can ignore it
   * @note the slot will be either player slot or -1, -1 means all players
   */
  virtual int RegisterPlayStartListener(PLAY_START_HANDLER handler) = 0;
  /*
   * @param id - listener id
   */
  virtual void UnregisterPlayStartListener(int id) = 0;
  /*
   * @param handler - play end handler
   * @return id - listener id, you can ignore it
   * @note the slot will be either player slot or -1, -1 means all players
   */
  virtual int RegisterPlayEndListener(PLAY_END_HANDLER handler) = 0;
  /*
   * @param id - listener id
   */
  virtual void UnregisterPlayEndListener(int id) = 0;
};
