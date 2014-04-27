#ifndef SUBTITLE_H_
#define SUBTITLE_H_
#include "json.h"
#include "window.h"
#include "scene.h"
#include <memory>
#include "queue.h"
namespace game
{
class Subtitle
{
public:
  Subtitle(json::JSON const& json, display::Window& window, Scene& scene, event::Queue& queue);
  void Text(std::string const& text);
  void Choice(std::string const& up, std::string const& down, std::string const& left, std::string const& right);
  void Clear(void);
  void Up(event::Command const& command);
  void Down(event::Command const& command);
  void Left(event::Command const& command);
  void Right(event::Command const& command);
private:
  std::shared_ptr<class SubtitleImpl> impl_;
};
}
#endif