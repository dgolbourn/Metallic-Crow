#include "mix_thread.h"
#include "SDL_mixer.h"
namespace mix
{
auto Mutex::lock() const -> void
{
  SDL_LockAudio();
}

auto Mutex::unlock() const noexcept -> void
{
  SDL_UnlockAudio();
}

Mutex mutex;
}