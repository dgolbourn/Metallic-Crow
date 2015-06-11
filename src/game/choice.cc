#include "choice.h"
#include "font.h"
#include "bind.h"
#include "position.h"
#include "animation.h"
#include "signal.h"
#include "timer.h"
#include <array>
namespace game
{
namespace
{
typedef std::array<display::Animation, 5> Animations;
typedef std::array<display::Animation::const_iterator, 5> Iterators;
typedef std::array<event::Signal, 5> Signals;
typedef std::array<bool, 5> Choices;
typedef std::array<display::BoundingBox, 5> Boxes;

typedef std::array<display::Texture, 4> Textures;
typedef std::array<game::Position, 4> Vectors;
typedef std::array<display::Modulation, 4> Modulations;
}

class Choice::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(lua::Stack& lua, display::Window& window, event::Queue& queue, boost::filesystem::path const& path);
  
  void Render() const;
  void Choice(std::string const& up, std::string const& down, std::string const& left, std::string const& right, double timer);
  
  template<int T> void Event();
  template<int T> void Event(event::Command const& command);
  template<int T> void Step();
  template<int T> void Modulation(float r, float g, float b, float a);

  void Fade();
  void Chosen();
  void Reset();

  void Pause();
  void Resume();
  
  bool paused_;
  display::Window window_;
  sdl::Font font_;
  Choices choices_;
  Textures text_;
  Animations icons_;
  Iterators current_icons_;
  Modulations modulation_; 
  Modulations current_modulation_;

  Signals signals_;
  
  event::Timer animation_timer_;
  event::Timer fade_timer_;
  Boxes icon_boxes_;
  Boxes text_boxes_;
  Vectors text_vectors_;
  display::Modulation current_fade_modulation_;
  display::Modulation fade_modulation_;
  int count_;
  event::Queue queue_;
  double interval_;
};

Choice::Impl::Impl(lua::Stack& lua, display::Window& window, event::Queue& queue, boost::filesystem::path const& path) : window_(window), paused_(true), count_(0), queue_(queue)
{
  {
    lua::Guard guard = lua.Field("interval");
    lua.Pop(interval_);
  }

  {
    lua::Guard guard = lua.Field("font");
    font_ = sdl::Font(lua, path);
  }

  static const std::string index[] = { "up", "down", "left", "right", "timer" };

  {
    lua::Guard guard = lua.Field("icons");
    for(int i = 0; i < 5; ++i)
    {
      lua::Guard guard = lua.Field(index[i]);
      icons_[i] = display::MakeAnimation(lua, window_, path);
      current_icons_[i] = icons_[i].cbegin();
    }
  }

  {
    lua::Guard guard = lua.Field("icon_boxes");
    for(int i = 0; i < 5; ++i)
    {
      lua::Guard guard = lua.Field(index[i]);
      icon_boxes_[i] = display::BoundingBox(lua);
    }
  }

  {
    lua::Guard guard = lua.Field("text_vectors");
    for(int i = 0; i < 4; ++i)
    {
      lua::Guard guard = lua.Field(index[i]);
      
      {
        lua::Guard guard = lua.Field(1);
        lua.Pop(text_vectors_[i].first);
      }

      {
        lua::Guard guard = lua.Field(2);
        lua.Pop(text_vectors_[i].second);
      }
    }
  }

  {
    lua::Guard guard = lua.Field("fade_modulation");
    fade_modulation_ = display::Modulation(lua);
  }
  current_fade_modulation_ = display::Modulation(1.f, 1.f, 1.f, 1.f);

  {
    lua::Guard guard = lua.Field("text_modulation");
    for(int i = 0; i < 4; ++i)
    {
      lua::Guard guard = lua.Field(index[i]);
      modulation_[i] = display::Modulation(lua);
      current_modulation_[i] = display::Modulation(modulation_[i].r(), modulation_[i].g(), modulation_[i].b(), modulation_[i].a());
    }
  }

  for(bool& choice : choices_)
  {
    choice = false;
  }
}

void Choice::Impl::Reset()
{
  for(bool& choice : choices_)
  {
    choice = false;
  }
  current_fade_modulation_.r(1.f);
  current_fade_modulation_.g(1.f);
  current_fade_modulation_.b(1.f);
  current_fade_modulation_.a(1.f);

  for(int i = 0; i < 4; ++i)
  {
    current_modulation_[i].r(modulation_[i].r());
    current_modulation_[i].g(modulation_[i].g());
    current_modulation_[i].b(modulation_[i].b());
    current_modulation_[i].a(modulation_[i].a());
  }

  count_ = 0;
  animation_timer_ = event::Timer();
  fade_timer_ = event::Timer();
  for(int i = 0; i < 5; ++i)
  {
    current_icons_[i] = icons_[i].cbegin();
  }
}

void Choice::Impl::Render() const
{
  for(int i = 0; i < 5; ++i)
  {
    if(choices_[i] && (current_icons_[i] != icons_[i].cend()))
    {
      (*current_icons_[i])(display::BoundingBox(), icon_boxes_[i], 0.f, false, 0., current_fade_modulation_);
    }
  }

  for(int i = 0; i < 4; ++i)
  {
    if(choices_[i])
    {
      text_[i](display::BoundingBox(), text_boxes_[i], 0.f, false, 0., current_modulation_[i]);
    }
  }
}

void Choice::Impl::Choice(std::string const& up, std::string const& down, std::string const& left, std::string const& right, double timer)
{
  Reset();

  std::array<std::string const*, 4> text = {&up, &down, &left, &right};
  for(int i = 0; i < 4; ++i)
  {
    choices_[i] = (*text[i] != "");
    if(choices_[i])
    {
      text_[i] = display::Texture(*text[i], font_, window_);
      
      display::Shape shape = text_[i].Shape();
      
      text_boxes_[i] = display::BoundingBox
      (
        icon_boxes_[i].x() + .5f * (icon_boxes_[i].w() * (1.f + text_vectors_[i].first) + shape.first * (text_vectors_[i].first - 1.f)),
        icon_boxes_[i].y() + .5f * (icon_boxes_[i].h() * (1.f + text_vectors_[i].second) + shape.second * (text_vectors_[i].second - 1.f)),
        shape.first, 
        shape.second
      );
    }
  }

  if(choices_[4] = (timer > 0.))
  {
    fade_timer_ = event::Timer(timer / 255., 255);
    fade_timer_.Add(function::Bind(&Impl::Fade, shared_from_this()));
    typedef void (Impl::*Notify)();
    fade_timer_.End(function::Bind((Notify)&Impl::Event<4>, shared_from_this()));
    queue_.Add(function::Bind(&event::Timer::operator(), fade_timer_));
    count_ = 0;
    if(!paused_)
    {
      fade_timer_.Resume();
    }
  }
}

template<int T> void Choice::Impl::Modulation(float r, float g, float b, float a)
{
  modulation_[T].r(r);
  modulation_[T].g(g);
  modulation_[T].b(b);
  modulation_[T].a(a);
  current_modulation_[T].r(r * current_fade_modulation_.r());
  current_modulation_[T].g(g * current_fade_modulation_.g());
  current_modulation_[T].b(b * current_fade_modulation_.b());
  current_modulation_[T].a(a * current_fade_modulation_.a());
}

template<int T> void Choice::Impl::Step()
{
  ++current_icons_[T];
}

template<int T> void Choice::Impl::Event()
{
  if(choices_[T] && !bool(animation_timer_))
  {
    if(icons_[T].size() >= 2)
    {
      animation_timer_ = event::Timer(interval_, icons_[T].size() - 2);
      animation_timer_.Add(function::Bind(&Impl::Step<T>, shared_from_this()));
      animation_timer_.End(function::Bind(&Impl::Reset, shared_from_this()));
      queue_.Add(function::Bind(&event::Timer::operator(), animation_timer_));
      fade_timer_ = event::Timer();
      if(!paused_)
      {
        animation_timer_.Resume();
      }
    }
    else
    {
      Reset();
    }

    signals_[T]();
  }
}

void Choice::Impl::Fade()
{
  ++count_;
  float fade = float(count_) / 255.f;
  current_fade_modulation_.r(1.f - fade + fade * fade_modulation_.r());
  current_fade_modulation_.g(1.f - fade + fade * fade_modulation_.g());
  current_fade_modulation_.b(1.f - fade + fade * fade_modulation_.b());
  current_fade_modulation_.a(1.f - fade + fade * fade_modulation_.a());

  for(int i = 0; i < 4; ++i)
  {
    current_modulation_[i].r(modulation_[i].r() * current_fade_modulation_.r());
    current_modulation_[i].g(modulation_[i].g() * current_fade_modulation_.g());
    current_modulation_[i].b(modulation_[i].b() * current_fade_modulation_.b());
    current_modulation_[i].a(modulation_[i].a() * current_fade_modulation_.a());
  }
}

void Choice::Impl::Pause()
{
  if(!paused_)
  {
    paused_ = true;
    if(animation_timer_)
    {
      animation_timer_.Pause();
    }
    if(fade_timer_)
    {
      fade_timer_.Pause();
    }
  }
}

void Choice::Impl::Resume()
{
  if(paused_)
  {
    paused_ = false;
    if(animation_timer_)
    {
      animation_timer_.Resume();
    }
    if(fade_timer_)
    {
      fade_timer_.Resume();
    }
  }
}

template<int T> void Choice::Impl::Event(event::Command const& command)
{
  signals_[T].Add(command);
}

void Choice::Up(event::Command const& command)
{
  impl_->Event<0>(command);
}

void Choice::Down(event::Command const& command)
{
  impl_->Event<1>(command);
}

void Choice::Left(event::Command const& command)
{
  impl_->Event<2>(command);
}

void Choice::Right(event::Command const& command)
{
  impl_->Event<3>(command);
}

void Choice::Timer(event::Command const& command)
{
  impl_->Event<4>(command);
}

void Choice::Up(float r, float g, float b, float a)
{
  impl_->Modulation<0>(r, g, b, a);
}

void Choice::Down(float r, float g, float b, float a)
{
  impl_->Modulation<1>(r, g, b, a);
}

void Choice::Left(float r, float g, float b, float a)
{
  impl_->Modulation<2>(r, g, b, a);
}

void Choice::Right(float r, float g, float b, float a)
{
  impl_->Modulation<3>(r, g, b, a);
}

void Choice::operator()(std::string const& up, std::string const& down, std::string const& left, std::string const& right, double timer)
{
  impl_->Choice(up, down, left, right, timer);
}

void Choice::Pause()
{
  impl_->Pause();
}

void Choice::Resume()
{
  impl_->Resume();
}

void Choice::Render() const
{
  impl_->Render();
}

void Choice::Up()
{
  impl_->Event<0>();
}

void Choice::Down()
{
  impl_->Event<1>();
}

void Choice::Left()
{
  impl_->Event<2>();
}

void Choice::Right()
{
  impl_->Event<3>();
}

Choice::operator bool() const
{
  return bool(impl_);
}

Choice::Choice(lua::Stack& lua, display::Window& window, event::Queue& queue, boost::filesystem::path const& path) : impl_(std::make_shared<Impl>(lua, window, queue, path))
{
}
}