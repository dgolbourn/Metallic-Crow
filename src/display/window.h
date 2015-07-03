#ifndef WINDOW_H_
#define WINDOW_H_
#include <string>
#include <memory>
#include "font.h"
#include "bounding_box.h"
#include "lua_stack.h"
#include "texture.h"
#include "weak_ptr.h"
namespace display
{
class Window
{
public:
  Window() = default;
  Window(lua::Stack& lua);
  auto Load(boost::filesystem::path const& file) -> void;
  auto Free(boost::filesystem::path const& file) -> void;
  auto Clear() const -> void;
  auto Show() const -> void;
  auto View(float x, float y, float zoom) -> void;
  auto Rotation(double angle) -> void;
  auto Draw(BoundingBox const& box, Modulation const& modulation) const -> void;
  typedef memory::WeakPtr<Window, class WindowImpl> WeakPtr;
  explicit operator bool() const;
private:
  std::shared_ptr<class WindowImpl> impl_;
  friend WeakPtr;
  friend class Texture::Impl;
};
}
#endif