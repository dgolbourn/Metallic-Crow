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
  void Load(boost::filesystem::path const& file);
  void Free(boost::filesystem::path const& file);
  void Clear() const;
  void Show() const;
  void View(float x, float y, float zoom);
  void Draw(BoundingBox const& box, Modulation const& modulation) const;
  typedef memory::WeakPtr<Window, class WindowImpl> WeakPtr;
  explicit operator bool() const;
private:
  std::shared_ptr<class WindowImpl> impl_;
  friend WeakPtr;
  friend class Texture::Impl;
};
}
#endif