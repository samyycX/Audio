#include <vector>
#ifdef PLATFORM_LINUX
#include <cstdint>
#endif
std::vector<uint8_t> convertAudioBufferToPCM(const std::vector<uint8_t> &inputBuffer);