#ifndef WINDOW_H_
#define WINDOW_H_
#include <string>
#include <memory>
#include "font.h"
#include "bounding_box.h"
#include "json.h"
#include "texture.h"
#include "weak_ptr.h"
#include "shape.h"
namespace display
{
class Window
{
public:
  Window() = default;
  Window(json::JSON const& json);
  void Free();
  void Clear() const;
  void Show() const;
  void View(float x, float y, float zoom);
  Shape Shape() const;
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