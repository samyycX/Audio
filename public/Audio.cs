using System.Reflection;
using System.Runtime.InteropServices;

public unsafe static class Audio
{
  public delegate void PlayStartHandler(int slot);
  public delegate void PlayEndHandler(int slot);

  private static class NativeMethods
  {
    [DllImport("audio", CallingConvention = CallingConvention.Cdecl)]
    public static extern void NativeSetPlayerHearing(int slot, bool hearing);

    [DllImport("audio", CallingConvention = CallingConvention.Cdecl)]
    public static extern void NativeSetAllPlayerHearing(bool hearing);

    [DllImport("audio", CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool NativeIsHearing(int slot);

    [DllImport("audio", CallingConvention = CallingConvention.Cdecl)]
    public static extern void NativePlayToPlayer(int slot, [MarshalAs(UnmanagedType.LPArray)] byte[] audioBuffer, int audioBufferSize, string audioPath, int audioPathSize, float volume = 1f);

    [DllImport("audio", CallingConvention = CallingConvention.Cdecl)]
    public static extern void NativePlay([MarshalAs(UnmanagedType.LPArray)] byte[] audioBuffer, int audioBufferSize, string audioPath, int audioPathSize, float volume = 1f);

    [DllImport("audio", CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool NativeIsPlaying(int slot);

    [DllImport("audio", CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool NativeIsAllPlaying();

    [DllImport("audio", CallingConvention = CallingConvention.Cdecl)]
    public static extern int NativeRegisterPlayStartListener([MarshalAs(UnmanagedType.FunctionPtr)] PlayStartHandler callback);

    [DllImport("audio", CallingConvention = CallingConvention.Cdecl)]
    public static extern void NativeUnregisterPlayStartListener(int id);

    [DllImport("audio", CallingConvention = CallingConvention.Cdecl)]
    public static extern int NativeRegisterPlayEndListener([MarshalAs(UnmanagedType.FunctionPtr)] PlayEndHandler callback);

    [DllImport("audio", CallingConvention = CallingConvention.Cdecl)]
    public static extern void NativeUnregisterPlayEndListener(int id);

    private static IntPtr DllImportResolver(string libraryName, Assembly assembly, DllImportSearchPath? searchPath)
    {
      if (libraryName == "audio")
      {
        return NativeLibrary.Load(RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ? "audio.dll" : "audio.so", assembly, searchPath);
      }

      return IntPtr.Zero;
    }

    private static void SetDllImportResolver()
    {
      NativeLibrary.SetDllImportResolver(Assembly.GetExecutingAssembly(), DllImportResolver);
    }

    static NativeMethods()
    {
      SetDllImportResolver();
    }
  }

  private static Dictionary<PlayStartHandler, int> _PlayStartListeners = new Dictionary<PlayStartHandler, int>();
  private static Dictionary<PlayEndHandler, int> _PlayEndListeners = new Dictionary<PlayEndHandler, int>();

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
  public static void PlayToPlayerFromBuffer(int slot, byte[] audioBuffer, float volume = 1f)
  {
    NativeMethods.NativePlayToPlayer(slot, audioBuffer, audioBuffer.Length, "", 0, volume);
  }

  /*
  * @param slot - player slot to set
  * @param audioFile - audio file path, must be absolute path to a audio file (like mp3, wav),
    will be decoded to pcm by ffmpeg, pass empty string means stop playing
  */
  public static void PlayToPlayerFromFile(int slot, string audioFile, float volume = 1f)
  {
    NativeMethods.NativePlayToPlayer(slot, [], 0, audioFile, audioFile.Length, volume);
  }

  /*
  * @param audioBuffer - buffer string, contains audio data (like mp3, wav), will be decoded to pcm by ffmpeg,
    pass empty string means stop playing
  */
  public static void PlayFromBuffer(byte[] audioBuffer, float volume = 1f)
  {
    NativeMethods.NativePlay(audioBuffer, audioBuffer.Length, "", 0, volume);
  }

  /*
  * @param audioFile - audio file path, must be absolute path to a audio file (like mp3, wav),
    will be decoded to pcm by ffmpeg, pass empty string means stop playing
  */
  public static void PlayFromFile(string audioFile, float volume = 1f)
  {
    NativeMethods.NativePlay([], 0, audioFile, audioFile.Length, volume);
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

  /*
  * @param handler - play start handler
  * @return id - listener id, you can ignore it
  * @note the slot will be either player slot or -1, -1 means all players
  */
  public static int RegisterPlayStartListener(PlayStartHandler handler)
  {
    var id = NativeMethods.NativeRegisterPlayStartListener(handler);
    _PlayStartListeners[handler] = id;
    return id;
  }

  /*
  * @param handler - play start handler
  */
  public static void UnregisterPlayStartListener(PlayStartHandler handler)
  {
    NativeMethods.NativeUnregisterPlayStartListener(_PlayStartListeners[handler]);
    _PlayStartListeners.Remove(handler);
  }

  /*
  * @param handler - play end handler
  * @return id - listener id, you can ignore it
  * @note the slot will be either player slot or -1, -1 means all players
  */
  public static int RegisterPlayEndListener(PlayEndHandler handler)
  {
    var id = NativeMethods.NativeRegisterPlayEndListener(handler);
    _PlayEndListeners[handler] = id;
    return id;
  }

  /*
  * @param handler - play end handler
  */
  public static void UnregisterPlayEndListener(PlayEndHandler handler)
  {
    NativeMethods.NativeUnregisterPlayEndListener(_PlayEndListeners[handler]);
    _PlayEndListeners.Remove(handler);
  }
}