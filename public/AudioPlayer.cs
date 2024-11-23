using System;
using System.Runtime.InteropServices;

public unsafe static class AudioPlayer
{
#if _WINDOWS
  private const string DllName = "audioplayer.dll";
#else
  private const string DllName = "audioplayer.so";
#endif

  private static class NativeMethods
  {
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void NativeSetPlayerHearing(int slot, bool hearing);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void NativeSetAllPlayerHearing(bool hearing);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool NativeIsHearing(int slot);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void NativeSetPlayerAudioBufferString(int slot, [MarshalAs(UnmanagedType.LPArray)] byte[] audioBuffer, int audioBufferSize, string audioPath, int audioPathSize);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void NativeSetAllAudioBufferString([MarshalAs(UnmanagedType.LPArray)] byte[] audioBuffer, int audioBufferSize, string audioPath, int audioPathSize);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool NativeIsPlaying(int slot);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool NativeIsAllPlaying();
  }

  /*
  * @param slot - player slot to set
  * @param hearing - whether player can hear
  */
  public static void SetPlayerHearing(int slot, bool hearing)
  {
    NativeMethods.NativeSetPlayerHearing(slot, hearing);
  }

  /*
  * @param hearing - whether all players can hear
  */
  public static void SetAllPlayerHearing(bool hearing)
  {
    NativeMethods.NativeSetAllPlayerHearing(hearing);
  }

  /*
  * @param slot - player slot to get
  * @return whether player can hear
  */
  public static bool IsHearing(int slot)
  {
    return NativeMethods.NativeIsHearing(slot);
  }

  /*
  * @param slot - player slot to set
  * @param audioBuffer - buffer string, contains audio data (like mp3, wav), will be decoded to pcm by ffmpeg,
    pass empty string means stop playing
  */
  public static void SetPlayerAudioBuffer(int slot, byte[] audioBuffer)
  {
    NativeMethods.NativeSetPlayerAudioBufferString(slot, audioBuffer, audioBuffer.Length, "", 0);
  }

  /*
  * @param slot - player slot to set
  * @param audioFile - audio file path, must be absolute path to a audio file (like mp3, wav),
    will be decoded to pcm by ffmpeg, pass empty string means stop playing
  */
  public static void SetPlayerAudioFile(int slot, string audioFile)
  {
    NativeMethods.NativeSetPlayerAudioBufferString(slot, [], 0, audioFile, audioFile.Length);
  }

  /*
  * @param audioBuffer - buffer string, contains audio data (like mp3, wav), will be decoded to pcm by ffmpeg,
    pass empty string means stop playing
  */
  public static void SetAllAudioBuffer(byte[] audioBuffer)
  {
    NativeMethods.NativeSetAllAudioBufferString(audioBuffer, audioBuffer.Length, "", 0);
  }

  /*
  * @param audioFile - audio file path, must be absolute path to a audio file (like mp3, wav),
    will be decoded to pcm by ffmpeg, pass empty string means stop playing
  */
  public static void SetAllAudioFile(string audioFile)
  {
    NativeMethods.NativeSetAllAudioBufferString([], 0, audioFile, audioFile.Length);
  }

  /*
  * @param slot - player slot to get
  * @return whether there are audio playing for a specific player
  */
  public static bool IsPlaying(int slot)
  {
    return NativeMethods.NativeIsPlaying(slot);
  }

  /*
  * @return whether there are audio playing for all players
  */
  public static bool IsAllPlaying()
  {
    return NativeMethods.NativeIsAllPlaying();
  }
}