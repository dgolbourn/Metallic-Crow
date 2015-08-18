#include "menu.h"
#include "texture.h"
#include "font.h"
#include "bounding_box.h"
#include <deque>
#include <map>
#include "signal.h"
#include "modulation.h"
namespace
{
struct Texture
{
  display::Texture texture_;
  display::BoundingBox render_box_;
  display::Modulation modulation_;
};

typedef std::deque<Texture> Textures;
typedef std::map<int, event::Signal> Signals;
}

namespace game
{ 
class Menu::Impl
{
public:
  Impl(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path);
  auto Add(int index, event::Command const& command) -> void;
  auto Previous() -> void;
  auto Next() -> void;
  auto Seek(int index) -> void;
  auto Select() -> void;
  auto Render() const -> void;
  auto SetUp() -> void;
  auto Choice(Options const& options) -> void;

  Texture background_;
  Textures idle_text_;
  Textures active_text_;
  Textures textures_;
  Signals signals_;
  int selection_;
  int selections_;
  sdl::Font idle_font_;
  sdl::Font active_font_;
  display::Window window_;
  boost::filesystem::path path_;
  display::Modulation idle_modulation_;
  display::Modulation active_modulation_;
};

Menu::Impl::Impl(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path) : selection_(0), selections_(0), window_(window), path_(path)
{
  {
    lua::Guard guard = lua.Field("idle_font");
    idle_font_ = sdl::Font(lua, path);
  }

  {
    lua::Guard guard = lua.Field("active_font");
    active_font_ = sdl::Font(lua, path);
  }

  {
    lua::Guard guard = lua.Field("active_font_modulation");
    active_modulation_ = display::Modulation(lua);
  }

  {
    lua::Guard guard = lua.Field("idle_font_modulation");
    idle_modulation_ = display::Modulation(lua);
  }

  display::BoundingBox clip;
  {
    lua::Guard guard = lua.Field("clip");
    clip = display::BoundingBox(lua);
  }

  background_.texture_ = display::Texture(display::Texture(path_ / lua.Field<std::string>("page"), window), clip);

  {
    lua::Guard guard = lua.Field("render_box");
    background_.render_box_ = display::BoundingBox(lua);
  }

  {
    lua::Guard guard = lua.Field("page_modulation");
    background_.modulation_ = display::Modulation(lua);
  }
}

auto Menu::Impl::Choice(Options const& options) -> void
{
  selection_ = 0;
  selections_ = static_cast<int>(options.size());

  idle_text_.clear();
  active_text_.clear();

  for(std::string const& option : options)
  {
    Texture idle;
    idle.texture_ = display::Texture(option, idle_font_, window_);
    idle.modulation_ = idle_modulation_;
    idle_text_.push_back(idle);

    Texture active;
    active.texture_ = display::Texture(option, active_font_, window_);
    active.modulation_ = active_modulation_;
    active_text_.push_back(active);
  }

  float line_spacing = 1.5f * idle_font_.LineSpacing();
  float height = 0.f;
  for(auto& text : idle_text_)
  {
    display::Shape shape = text.texture_.Shape();
    float current = 0.f;
    while(current < shape.second)
    {
      current += line_spacing - shape.second;
    }
    height += current;
  }

  float y = background_.render_box_.y() + .5f * (background_.render_box_.h() - height);
  for(auto& text : idle_text_)
  {
    display::Shape shape = text.texture_.Shape();
    text.render_box_ = display::BoundingBox(background_.render_box_.x() + .5f * (background_.render_box_.w() - shape.first), y, shape.first, shape.second);

    float current = line_spacing;
    while(current < shape.second)
    {
      current += line_spacing - shape.second;
    }
    y += current;
  }

  for(int i = 0; i < selections_; ++i)
  {
    display::Shape shape = active_text_[i].texture_.Shape();
    display::BoundingBox box = idle_text_[i].render_box_;
    active_text_[i].render_box_ = display::BoundingBox(box.x() + .5f * (box.w() - shape.first), box.y() + .5f * (box.h() - shape.second), shape.first, shape.second);
  }

  SetUp();
}

auto Menu::Impl::SetUp() -> void
{
  textures_ = idle_text_;
  if(selections_ > 0)
  {
    textures_[selection_] = active_text_[selection_];
    std::swap(textures_[selection_], textures_.back());
  }
  textures_.push_front(background_);
}

auto Menu::Impl::Add(int index, event::Command const& command) -> void
{
  signals_[index].Add(command);
}

auto Menu::Impl::Previous() -> void
{
  if(selection_ > 0)
  {
    --selection_;
    SetUp();
  }
}

auto Menu::Impl::Next() -> void
{
  if(selection_ < (selections_ - 1))
  { 
    ++selection_;
    SetUp();
  }
}

auto Menu::Impl::Seek(int index) -> void
{
  if((index != selection_) && (index >= 0) && (index < selections_))
  {
    selection_ = index;
    SetUp();
  }
}

auto Menu::Impl::Select() -> void
{
  if(selections_ > 0)
  {
    signals_[selection_]();
  }
}

auto Menu::Impl::Render() const -> void
{
  for(auto& texture : textures_)
  {
    texture.texture_(display::BoundingBox(), texture.render_box_, 0.f, false, 0., texture.modulation_);
  }
}

auto Menu::Add(int index, event::Command const& command) -> void
{
  impl_->Add(index, command);
}

auto Menu::Previous() -> void
{
  impl_->Previous();
}

auto Menu::Next() -> void
{
  impl_->Next();
}

auto Menu::operator[](int index) -> void
{
  impl_->Seek(index);
}

auto Menu::Select() -> void
{
  impl_->Select();
}

auto Menu::Render() const -> void
{
  impl_->Render();
}

auto Menu::operator()(Options const& options) -> void
{
  impl_->Choice(options);
}

Menu::Menu(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path) : impl_(std::make_shared<Impl>(lua, window, path))
{
}
}