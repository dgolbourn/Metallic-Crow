#ifndef SOUND_H_
#define SOUND_H_

#include <memory>
#include <string>

namespace audio
{
class Sound
{
public:
  Sound(void) = default;
  Sound(std::string const& filename);
  void Play(int repeats = 0);
  void Pause(void) const;
  void Resume(void) const;
  void Stop(void) const;
  void Fade(int ms) const;
  void Volume(int volume);
private:
  std::shared_ptr<class SoundImpl> impl_;
};
}
#endif