#ifndef SUBTITLE_H_
#define SUBTITLE_H_
#include "json.h"
#include "window.h"
#include "scene.h"
#include <memory>
#include "queue.h"
#include "weak_ptr.h"
namespace game
{
class Subtitle
{
public:
  Subtitle(void) = default;
  Subtitle(json::JSON const& json, display::Window& window, event::Queue& queue);
  void Text(std::string const& text);
  void Choice(std::string const& up, std::string const& down, std::string const& left, std::string const& right);
  void Up(event::Command const& command);
  void Down(event::Command const& command);
  void Left(event::Command const& command);
  void Right(event::Command const& command);
  void Pause(void);
  void Resume(void);
  void Render(void) const;
  void Up(void);
  void Down(void);
  void Left(void);
  void Right(void);
  explicit operator bool(void) const;
  typedef memory::WeakPtr<Subtitle, class SubtitleImpl> WeakPtr;
private:
  std::shared_ptr<class SubtitleImpl> impl_;
  friend WeakPtr;
};
}
#endif