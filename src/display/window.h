#ifndef WINDOW_H_
#define WINDOW_H_
#include <string>
#include <memory>
#include "font.h"
#include "bounding_box.h"
#include "json.h"
#include "texture.h"
#include "weak_ptr.h"
namespace display
{
class Window
{
public:
  Window(void) = default;
  Window(json::JSON const& json);
  void Free(void);
  void Clear(void) const;
  void Show(void) const;
  void View(float x, float y, float zoom);
  typedef memory::WeakPtr<Window, class WindowImpl> WeakPtr;
  explicit operator bool(void) const;
private:
  std::shared_ptr<class WindowImpl> impl_;
  friend WeakPtr;
  friend class TextureImpl;
};
}
#endif