#include "menu.h"
#include "json_iterator.h"
#include "texture.h"
#include "font.h"
#include "bounding_box.h"
#include <deque>
#include <map>
#include "signal.h"
namespace game
{ 
namespace
{
typedef std::pair<display::Texture, display::BoundingBox> Texture;
typedef std::deque<Texture> Textures;
typedef std::map<int, event::Signal> Signals;
}

class Menu::Impl
{
public:
  Impl(json::JSON const& json, display::Window& window);
  void Add(int index, event::Command const& command);
  void Previous();
  void Next();
  void Seek(int index);
  void Select();
  void Render() const;
  void SetUp();
  void Choice(Options const& options);

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
};

Menu::Impl::Impl(json::JSON const& json, display::Window& window) : selection_(0), selections_(0), window_(window)
{
  json_t* idle_ref;
  json_t* active_ref;
  char const* background_file;
  json_t* clip;
  json_t* render_box_ref;

  json.Unpack("s{soso}sssoso",
    "fonts",
    "idle", &idle_ref,
    "active", &active_ref,
    "page", &background_file,
    "clip", &clip,
    "render_box", &render_box_ref);

  idle_font_ = sdl::Font(json::JSON(idle_ref));
  active_font_ = sdl::Font(json::JSON(active_ref));

  background_ = Texture(display::Texture(display::Texture(background_file, window), display::BoundingBox(json::JSON(clip))), display::BoundingBox(json::JSON(render_box_ref)));
}

void Menu::Impl::Choice(Options const& options)
{
  selection_ = 0;
  selections_ = int(options.size());

  idle_text_.clear();
  active_text_.clear();

  for(std::string const& option : options)
  {
    idle_text_.emplace_back(display::Texture(option, idle_font_, window_), display::BoundingBox());
    active_text_.emplace_back(display::Texture(option, active_font_, window_), display::BoundingBox());
  }

  float line_spacing = idle_font_.LineSpacing();
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

  for(int i = 0; i < selections_; ++i)
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
  if(selections_ > 0)
  {
    textures_[selection_] = active_text_[selection_];
    std::swap(textures_[selection_], textures_.back());
  }
  textures_.push_front(background_);
}

void Menu::Impl::Add(int index, event::Command const& command)
{
  signals_[index].Add(command);
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
  if(selection_ < (selections_ - 1))
  { 
    ++selection_;
    SetUp();
  }
}

void Menu::Impl::Seek(int index)
{
  if((index != selection_) && (index >= 0) && (index < selections_))
  {
    selection_ = index;
    SetUp();
  }
}

void Menu::Impl::Select()
{
  if(selections_ > 0)
  {
    signals_[selection_]();
  }
}

void Menu::Impl::Render() const
{
  for(auto& texture : textures_)
  {
    texture.first(display::BoundingBox(), texture.second, 0.f, false, 0., display::Modulation());
  }
}

void Menu::Add(int index, event::Command const& command)
{
  impl_->Add(index, command);
}

void Menu::Previous()
{
  impl_->Previous();
}

void Menu::Next()
{
  impl_->Next();
}

void Menu::operator[](int index)
{
  impl_->Seek(index);
}

void Menu::Select()
{
  impl_->Select();
}

void Menu::Render() const
{
  impl_->Render();
}

void Menu::operator()(Options const& options)
{
  impl_->Choice(options);
}

Menu::Menu(json::JSON const& json, display::Window& window) : impl_(std::make_shared<Impl>(json, window))
{
}
}