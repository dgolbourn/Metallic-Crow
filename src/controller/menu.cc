#include "menu.h"
#include "json_iterator.h"
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
  Impl(json::JSON const& json, display::Window& window, boost::filesystem::path const& path);
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
  boost::filesystem::path path_;
  display::Modulation idle_modulation_;
  display::Modulation active_modulation_;
};

Menu::Impl::Impl(json::JSON const& json, display::Window& window, boost::filesystem::path const& path) : selection_(0), selections_(0), window_(window), path_(path)
{
  json_t* idle_ref;
  json_t* active_ref;
  char const* background_file;
  json_t* clip;
  json_t* render_box_ref;
  json_t* idle_mod;
  json_t* active_mod;
  json_t* page_mod;

  json.Unpack("{sososososssososo}",
    "idle font", &idle_ref,
    "idle font modulation", &idle_mod,
    "active font", &active_ref,
    "active font modulation", &active_mod,
    "page", &background_file,
    "clip", &clip,
    "render box", &render_box_ref,
    "page modulation", &page_mod);

  idle_font_ = sdl::Font(json::JSON(idle_ref), path);
  active_font_ = sdl::Font(json::JSON(active_ref), path);

  active_modulation_ = display::Modulation(json::JSON(active_mod));
  idle_modulation_ = display::Modulation(json::JSON(idle_mod));
  background_.texture_ = display::Texture(display::Texture(path_ / background_file, window), display::BoundingBox(json::JSON(clip)));
  background_.render_box_ = display::BoundingBox(json::JSON(render_box_ref));
  background_.modulation_ = display::Modulation(json::JSON(page_mod));
}

void Menu::Impl::Choice(Options const& options)
{
  selection_ = 0;
  selections_ = int(options.size());

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
    texture.texture_(display::BoundingBox(), texture.render_box_, 0.f, false, 0., texture.modulation_);
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

Menu::Menu(json::JSON const& json, display::Window& window, boost::filesystem::path const& path) : impl_(std::make_shared<Impl>(json, window, path))
{
}
}