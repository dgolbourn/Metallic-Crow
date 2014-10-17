#ifndef SAVES_H_
#define SAVES_H_
#include <string>
#include <memory>
#include "boost/optional.hpp"
namespace game
{ 
class Saves
{
public:
  Saves() = default;
  Saves(std::string const& file);
  typedef boost::optional<int> Slot;
  bool Playing() const;
  int LastPlayed() const;
  int Size() const;
  int Progress(int slot) const;
  int Current(int slot) const;
  std::string LastPlayed(int slot) const;
  void Play(int slot);
  void Stop();
  void Save();
  void Current(int slot, int current);
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif