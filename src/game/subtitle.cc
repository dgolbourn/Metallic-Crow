#include "subtitle.h"
namespace game
{
class Subtitle::Impl
{
public:
  Impl(json::JSON const& json, display::Window& window, boost::filesystem::path const& path);
  void Subtitle(std::string const& text);
  void Modulation(float r, float g, float b, float a);
  void Render(void) const;

  sdl::Font font_;
  display::Modulation modulation_;
  display::BoundingBox clip_;
  display::BoundingBox render_box_;
  display::Texture text_;
  display::Window window_;
};

Subtitle::Impl::Impl(json::JSON const& json, display::Window& window, boost::filesystem::path const& path) : window_(window)
{
  json_t* font;
  json_t* clip;
  
  json.Unpack("{soso}",
    "font", &font,
    "render box", &clip);

  font_ = sdl::Font(json::JSON(font), path);
  clip_ = display::BoundingBox(json::JSON(clip));
}

void Subtitle::Impl::Render() const
{
  text_(display::BoundingBox(), render_box_, 0.f, false, 0., modulation_);
}

void Subtitle::Impl::Subtitle(std::string const& text)
{
  text_ = display::Texture(display::Texture(text, font_, clip_.w(), window_), display::BoundingBox(0.f, 0.f, clip_.w(), clip_.h()));

  display::Shape shape = text_.Shape();
  render_box_ = display::BoundingBox(clip_.x() + .5f * (clip_.w() - shape.first), clip_.y() + .5f * (clip_.h() - shape.second), shape.first, shape.second);
}

void Subtitle::Impl::Modulation(float r, float g, float b, float a)
{
  modulation_ = display::Modulation(r, g, b, a);
}

void Subtitle::Render() const
{
  impl_->Render();
}

void Subtitle::operator()(std::string const& text)
{
  impl_->Subtitle(text);
}

void Subtitle::Modulation(float r, float g, float b, float a)
{
  impl_->Modulation(r, g, b, a);
}

Subtitle::Subtitle(json::JSON const& json, display::Window& window, boost::filesystem::path const& path) : impl_(std::make_shared<Impl>(json, window, path))
{
}
}