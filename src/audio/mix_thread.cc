#include "mix_thread.h"
#include "SDL_mixer.h"
namespace mix
{
void Mutex::lock() const
{
  SDL_LockAudio();
}

void Mutex::unlock() const noexcept
{
  SDL_UnlockAudio();
}

Mutex mutex;
}