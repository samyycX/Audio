#include <vector>
#include <iostream>
#ifdef PLATFORM_LINUX
#include <cstdint>
#endif
std::vector<uint8_t> convertAudioBufferToPCM(std::string file_path);