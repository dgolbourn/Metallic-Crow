#ifndef AVATAR_H_
#define AVATAR_H_
#include <memory>
#include "window.h"
#include "json.h"
#include "queue.h"
#include "position.h"
#include "boost/filesystem.hpp"
namespace game
{
class Avatar
{
public:
  Avatar(void) = default;
  Avatar(json::JSON const& json, display::Window& window, event::Queue& queue, boost::filesystem::path const& path);
  void Eyes(std::string const& expression);
  void Mouth(std::string const& expression);
  void Body(std::string const& expression);
  void Mouth(int open);
  void Eyes(int open);
  bool Eyes() const;
  void Facing(bool left_facing);
  void Position(game::Position const& position);
  void Modulation(display::Modulation const& modulation);
  void Pause(void);
  void Resume(void);
  void Render(void) const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif