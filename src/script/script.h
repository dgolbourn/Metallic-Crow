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
  auto Pause() -> void;
  auto Resume() -> void;
  auto Render() -> void;
  auto ChoiceUp() -> void;
  auto ChoiceDown() -> void;
  auto ChoiceLeft() -> void;
  auto ChoiceRight() -> void;
  auto Control(float x, float y) -> void;
  auto Look(float x, float y) -> void;
  auto Add(event::Command const& command) -> void;
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