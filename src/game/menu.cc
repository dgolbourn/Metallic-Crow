#include "menu.h"
#include "json_iterator.h"
#include "texture.h"
#include "font.h"
#include "bounding_box.h"
#include <vector>
#include <deque>
#include "signal.h"
namespace game
{ 
namespace
{
typedef std::pair<display::Texture, display::BoundingBox> Texture;
typedef std::deque<Texture> Textures;
typedef std::vector<event::Signal> Signals;
}

class Menu::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(json::JSON const& json, display::Window& window);
  void Add(int selection, event::Command const& command);
  void Previous();
  void Next();
  void Select();
  void Render() const;
  void SetUp();

  Texture background_;
  Textures idle_text_;
  Textures active_text_;
  Textures textures_;
  Signals signals_;
  int selection_;
};

Menu::Impl::Impl(json::JSON const& json, display::Window& window) : selection_(0)
{
  json_t* idle_ref;
  json_t* active_ref;
  char const* background_file;
  json_t* clip;
  json_t* render_box_ref;
  json_t* options;

  json.Unpack("s{soso}sssososo",
    "fonts", 
    "idle", &idle_ref,
    "active", &active_ref,
    "page", &background_file,
    "clip", &clip,
    "render_box", &render_box_ref,
    "options", &options);

  sdl::Font idle_font((json::JSON(idle_ref)));
  sdl::Font active_font((json::JSON(active_ref)));

  background_ = Texture(display::Texture(display::Texture(background_file, window), display::BoundingBox(json::JSON(clip))), display::BoundingBox(json::JSON(render_box_ref)));

  signals_.resize(json::JSON(options).Size());

  for(json::JSON const& option : json::JSON(options))
  {
    char const* text;
    option.Unpack("s", &text);

    idle_text_.emplace_back(display::Texture(text, idle_font, window), display::BoundingBox());
    active_text_.emplace_back(display::Texture(text, active_font, window), display::BoundingBox());
  }

  float line_spacing = idle_font.LineSpacing();
  float height = 0.f;
  for(auto& text : idle_text_)
  {
    display::Shape shape = text.first.Shape();
    float current = line_spacing;
    while(current < shape.second)
    {
      current += line_spacing;
    }
    height += current;
  }

  float y = background_.second.y() + .5f * (background_.second.h() - height);
  for(auto& text : idle_text_)
  {
    display::Shape shape = text.first.Shape();
    text.second = display::BoundingBox(background_.second.x() + .5f * (background_.second.w() - shape.first), y, shape.first, shape.second);

    float current = line_spacing;
    while(current < shape.second)
    {
      current += line_spacing;
    }
    y += current;
  }

  for(Signals::size_type i = 0; i < signals_.size(); ++i)
  {
    display::Shape shape = active_text_[i].first.Shape();
    display::BoundingBox box = idle_text_[i].second;
    active_text_[i].second = display::BoundingBox(box.x() + .5f * (box.w() - shape.first), box.y() + .5f * (box.h() - shape.second), shape.first, shape.second);
  }

  SetUp();
}

void Menu::Impl::SetUp()
{
  textures_ = idle_text_;
  textures_[selection_] = active_text_[selection_];
  std::swap(textures_[selection_], textures_.back());
  textures_.push_front(background_);
}

void Menu::Impl::Add(int selection, event::Command const& command)
{
  if((selection >= 0) && (selection < int(signals_.size())))
  {
    signals_[selection].Add(command);
  }
}

void Menu::Impl::Previous()
{
  if(selection_ > 0)
  {
    --selection_;
    SetUp();
  }
}

void Menu::Impl::Next()
{
  if(selection_ < (int(signals_.size()) - 1))
  { 
    ++selection_;
    SetUp();
  }
}

void Menu::Impl::Select()
{
  signals_[selection_]();
}

void Menu::Impl::Render() const
{
  for(auto& texture : textures_)
  {
    texture.first(display::BoundingBox(), texture.second, 0.f, false, 0., display::Modulation());
  }
}

void Menu::Add(int selection, event::Command const& command)
{
  impl_->Add(selection, command);
}

void Menu::Previous()
{
  impl_->Previous();
}

void Menu::Next()
{
  impl_->Next();
}

void Menu::Select()
{
  impl_->Select();
}

void Menu::Render() const
{
  impl_->Render();
}

Menu::Menu(json::JSON const& json, display::Window& window) : impl_(std::make_shared<Impl>(json, window))
{
}
}