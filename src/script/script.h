#ifndef SCRIPT_H_
#define SCRIPT_H_
#include <memory>
#include <string>
#include "window.h"
#include "weak_ptr.h"
#include "queue.h"
#include "boost/filesystem.hpp"
namespace game
{
class Script
{
public:
  Script() = default;
  Script(boost::filesystem::path const& file, display::Window& window, event::Queue& queue, boost::filesystem::path const& path, float volume);
  void Pause();
  void Resume();
  void Render();
  void ChoiceUp();
  void ChoiceDown();
  void ChoiceLeft();
  void ChoiceRight();
  void Control(float x, float y);
  void Add(event::Command const& command);
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Script> WeakPtr;
  friend WeakPtr;
};
}
#endif