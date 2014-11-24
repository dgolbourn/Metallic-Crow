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

game::Position MakeVector(json::JSON const& json)
{
  double x, y;
  json.Unpack("[ff]", &x, &y);
  return game::Position(float(x), float(y));
}
}

class Choice::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(json::JSON const& json, display::Window& window, event::Queue& queue, boost::filesystem::path const& path);
  
  void Render() const;
  void Choice(std::string const& up, std::string const& down, std::string const& left, std::string const& right, double timer);
  
  template<int T> void Event();
  template<int T> void Event(event::Command const& command);
  template<int T> void Step();

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
  
  Signals signals_;
  
  event::Timer animation_timer_;
  event::Timer fade_timer_;
  Boxes icon_boxes_;
  Boxes text_boxes_;
  Vectors text_vectors_;
  display::Modulation modulation_;
  display::Modulation end_;
  display::Modulation start_;
  int count_;
  event::Queue queue_;
  double interval_;
};

Choice::Impl::Impl(json::JSON const& json, display::Window& window, event::Queue& queue, boost::filesystem::path const& path) : window_(window), paused_(true), count_(0), queue_(queue), modulation_(1.f, 1.f, 1.f, 1.f)
{
  json_t* font;
  json_t* icon[5];
  json_t* box[5];
  json_t* vector[4];
  double sr, sg, sb, sa;
  double er, eg, eb, ea;
  
  json.Unpack("{sos{sososososo}s{sososososo}s{sosososo}sfs[ffff]s[ffff]}",
    "font", &font,
    "icons", 
    "up", &icon[0],
    "down", &icon[1],
    "left", &icon[2],
    "right", &icon[3],
    "timer", &icon[4],
    "icon boxes",
    "up", &box[0],
    "down", &box[1],
    "left", &box[2],
    "right", &box[3],
    "timer", &box[4],
    "text vectors",
    "up", &vector[0],
    "down", &vector[1],
    "left", &vector[2],
    "right", &vector[3],
    "interval", &interval_,
    "start modulation", &sr, &sg, &sb, &sa,
    "end modulation", &er, &eg, &eb, &ea);

  font_ = sdl::Font(json::JSON(font), path);

  for(int i = 0; i < 5; ++i)
  {
    icons_[i] = display::MakeAnimation(json::JSON(icon[i]), window_, path);
    current_icons_[i] = icons_[i].cbegin();
    icon_boxes_[i] = display::BoundingBox(json::JSON(box[i]));
  }
  
  for(int i = 0; i < 4; ++i)
  {
    text_vectors_[i] = MakeVector(json::JSON(vector[i]));
  }

  start_ = display::Modulation(float(sr), float(sg), float(sb), float(sa));
  end_ = display::Modulation(float(er), float(eg), float(eb), float(ea));
}

void Choice::Impl::Reset()
{
  for(bool& choice : choices_)
  {
    choice = false;
  }
  modulation_.r(start_.r());
  modulation_.g(start_.g());
  modulation_.b(start_.b());
  modulation_.a(start_.a());
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
      (*current_icons_[i])(display::BoundingBox(), icon_boxes_[i], 0.f, false, 0., display::Modulation());
    }
  }

  for(int i = 0; i < 4; ++i)
  {
    if(choices_[i])
    {
      text_[i](display::BoundingBox(), text_boxes_[i], 0.f, false, 0., modulation_);
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

template<int T> void Choice::Impl::Step()
{
  ++current_icons_[T];
}

template<int T> void Choice::Impl::Event()
{
  if(choices_[T] && !bool(animation_timer_))
  {
    signals_[T]();
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
  }
}

void Choice::Impl::Fade()
{
  ++count_;
  float f = count_ / 255.f;
  float g = 1.f - f;
  modulation_ = display::Modulation
  (
    f * end_.r() + g * start_.r(),
    f * end_.g() + g * start_.g(),
    f * end_.b() + g * start_.b(),
    f * end_.a() + g * start_.a()
  );
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

Choice::Choice(json::JSON const& json, display::Window& window, event::Queue& queue, boost::filesystem::path const& path) : impl_(std::make_shared<Impl>(json, window, queue, path))
{
}
}