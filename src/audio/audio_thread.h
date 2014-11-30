#ifndef AUDIO_THREAD_H_
#define AUDIO_THREAD_H_
#include <mutex>
namespace audio
{
extern std::recursive_mutex mutex;
typedef std::lock_guard<std::recursive_mutex> Guard;
}
#endif