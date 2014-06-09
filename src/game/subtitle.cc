#include "subtitle.h"
#include "font.h"
#include "bind.h"
#include "position.h"
#include "state.h"
#include "signal.h"
namespace game
{
class SubtitleImpl final : public std::enable_shared_from_this<SubtitleImpl>
{
public:
  SubtitleImpl(json::JSON const& json, display::Window& window, event::Queue& queue);
  void Render(void);
  void Init(event::Event& event);
  void Text(std::string const& text);
  void Choice(std::string const& up, std::string const& down, std::string const& left, std::string const& right);
  void UpEvent(void);
  void DownEvent(void);
  void LeftEvent(void);
  void RightEvent(void);
  void Pause(void);
  void Resume(void);
  void Change(display::BoundingBox& box, State& current, State& next, display::BoundingBox& offset);
  void Up(event::Command const& command);
  void Down(event::Command const& command);
  void Left(event::Command const& command);
  void Right(event::Command const& command);
  void Active(void);
  display::BoundingBox Update(State& state, display::BoundingBox& offset);
  display::Window window_;
  sdl::Font text_font_;
  sdl::Font choice_font_;
  int length_;
  bool active_;
  bool subtitle_;
  bool up_choice_;
  bool down_choice_;
  bool left_choice_;
  bool right_choice_;
  bool paused_;
  display::Texture text_;
  display::Texture up_;
  display::Texture down_;
  display::Texture left_;
  display::Texture right_;
  State up_idle_;
  State down_idle_;
  State left_idle_;
  State right_idle_;
  State up_active_;
  State down_active_;
  State left_active_;
  State right_active_;
  State up_current_;
  State down_current_;
  State left_current_;
  State right_current_;
  event::Signal up_signal_;
  event::Signal down_signal_;
  event::Signal left_signal_;
  event::Signal right_signal_;
  display::BoundingBox text_box_;
  display::BoundingBox up_box_;
  display::BoundingBox up_icon_box_;
  display::BoundingBox up_offset_;
  display::BoundingBox down_box_;
  display::BoundingBox down_icon_box_;
  display::BoundingBox down_offset_;
  display::BoundingBox left_box_;
  display::BoundingBox left_icon_box_;
  display::BoundingBox left_offset_;
  display::BoundingBox right_box_;
  display::BoundingBox right_icon_box_;
  display::BoundingBox right_offset_;
  display::BoundingBox text_offset_;
  display::BoundingBox up_text_offset_;
  display::BoundingBox down_text_offset_;
  display::BoundingBox left_text_offset_;
  display::BoundingBox right_text_offset_;
};

SubtitleImpl::SubtitleImpl(json::JSON const& json, display::Window& window, event::Queue& queue) : window_(window), subtitle_(false), up_choice_(false), down_choice_(false), left_choice_(false), right_choice_(false), active_(true), paused_(true)
{
  json_t* text_font;
  json_t* choice_font;
  json_t* up_idle;
  json_t* down_idle;
  json_t* left_idle;
  json_t* right_idle;
  json_t* up_active;
  json_t* down_active;
  json_t* left_active;
  json_t* right_active;
  double base_offset, text_offset, choice_icon_offset, choice_text_offset;

  json.Unpack("{sososisfsfsfsfsosososososososo}",
    "text font", &text_font,
    "choice font", &choice_font,
    "length", &length_,
    "base offset", &base_offset,
    "text offset", &text_offset,
    "choice icon offset", &choice_icon_offset,
    "choice text offset", &choice_text_offset,
    "up idle", &up_idle,
    "down idle", &down_idle,
    "left idle", &left_idle,
    "right idle", &right_idle,
    "up active", &up_active,
    "down active", &down_active,
    "left active", &left_active,
    "right active", &right_active);
  text_font_ = sdl::Font(text_font);
  choice_font_ = sdl::Font(choice_font);
  up_idle_ = State(up_idle, window_, queue);
  down_idle_ = State(down_idle, window_, queue);
  left_idle_ = State(left_idle, window_, queue);
  right_idle_ = State(right_idle, window_, queue);
  up_active_ = State(up_active, window_, queue);
  down_active_ = State(down_active, window_, queue);
  left_active_ = State(left_active, window_, queue);
  right_active_ = State(right_active, window_, queue);
  up_current_ = up_idle_;
  down_current_ = down_idle_;
  left_current_ = left_idle_;
  right_current_ = right_idle_;
  up_current_.Play();
  up_current_.Pause();
  down_current_.Play();
  down_current_.Pause();
  left_current_.Play();
  left_current_.Pause();
  right_current_.Play();
  right_current_.Pause();
  display::Shape shape = window_.Shape();
  float x = .5f * shape.first;
  float y = shape.second - float(base_offset);
  up_offset_ = display::BoundingBox(x, y - float(choice_icon_offset), 0.f, 0.f);
  down_offset_ = display::BoundingBox(x, y + float(choice_icon_offset), 0.f, 0.f);
  left_offset_ = display::BoundingBox(x - float(choice_icon_offset), y, 0.f, 0.f);
  right_offset_ = display::BoundingBox(x + float(choice_icon_offset), y, 0.f, 0.f);
  up_icon_box_ = Update(up_current_, up_offset_);
  down_icon_box_ = Update(down_current_, down_offset_);
  left_icon_box_ = Update(left_current_, left_offset_);
  right_icon_box_ = Update(right_current_, right_offset_);
  text_offset_ = display::BoundingBox(x, y - float(text_offset), 0.f, 0.f);
  up_text_offset_ = display::BoundingBox(x, y - float(choice_text_offset), 0.f, 0.f);
  down_text_offset_ = display::BoundingBox(x, y + float(choice_text_offset), 0.f, 0.f);
  left_text_offset_ = display::BoundingBox(x - float(choice_text_offset), y, 0.f, 0.f);
  right_text_offset_ = display::BoundingBox(x + float(choice_text_offset), y, 0.f, 0.f);
}

void SubtitleImpl::Init(event::Event& event)
{
  auto ptr = shared_from_this();
  event.ChoiceUp(event::Bind(&SubtitleImpl::UpEvent, ptr), event::None);
  event.ChoiceDown(event::Bind(&SubtitleImpl::DownEvent, ptr), event::None);
  event.ChoiceLeft(event::Bind(&SubtitleImpl::LeftEvent, ptr), event::None);
  event.ChoiceRight(event::Bind(&SubtitleImpl::RightEvent, ptr), event::None);
  up_signal_.Add(event::Bind(&SubtitleImpl::Active, ptr));
  down_signal_.Add(event::Bind(&SubtitleImpl::Active, ptr));
  left_signal_.Add(event::Bind(&SubtitleImpl::Active, ptr));
  right_signal_.Add(event::Bind(&SubtitleImpl::Active, ptr));
}

void SubtitleImpl::Active(void)
{
  active_ = true;
  subtitle_ = up_choice_ = down_choice_ = left_choice_ = right_choice_ = false;
  Change(up_icon_box_, up_current_, up_idle_, up_offset_);
  Change(down_icon_box_, down_current_, down_idle_, down_offset_);
  Change(left_icon_box_, left_current_, left_idle_, left_offset_);
  Change(right_icon_box_, right_current_, right_idle_, right_offset_);
}

void SubtitleImpl::Render(void)
{
  if(subtitle_)
  {
    text_(display::BoundingBox(), text_box_, 0.f, false, 0.);
  }

  if(up_choice_)
  {
    up_(display::BoundingBox(), up_box_, 0.f, false, 0.);
    up_current_.Render(up_icon_box_, 0.f, false, 0.);
  }

  if(down_choice_)
  {
    down_(display::BoundingBox(), down_box_, 0.f, false, 0.);
    down_current_.Render(down_icon_box_, 0.f, false, 0.);
  }

  if(left_choice_)
  {
    left_(display::BoundingBox(), left_box_, 0.f, false, 0.);
    left_current_.Render(left_icon_box_, 0.f, false, 0.);
  }

  if(right_choice_)
  {
    right_(display::BoundingBox(), right_box_, 0.f, false, 0.);
    right_current_.Render(right_icon_box_, 0.f, false, 0.);
  }
}

void SubtitleImpl::Text(std::string const& text)
{
  subtitle_ = false;
  if(text != "")
  {
    text_ = display::Texture(text, text_font_, length_, window_);
    subtitle_ = true;
    display::Shape shape = text_.Shape();
    text_box_ = text_offset_.Copy();
    text_box_.x(text_box_.x() - .5f * shape.first);
    text_box_.y(text_box_.y() - .5f * shape.second);
  }
}

void SubtitleImpl::Choice(std::string const& up, std::string const& down, std::string const& left, std::string const& right)
{
  active_ = true;

  up_choice_ = false;
  if(up != "")
  {
    up_ = display::Texture(up, choice_font_, window_);
    up_choice_ = true;
    display::Shape shape = up_.Shape();
    up_box_ = up_text_offset_.Copy();
    up_box_.x(up_box_.x() - .5f * shape.first);
    up_box_.y(up_box_.y() - shape.second);
    up_icon_box_ = Update(up_current_, up_offset_);
  }

  down_choice_ = false;
  if(down != "")
  {
    down_ = display::Texture(down, choice_font_, window_);
    down_choice_ = true;
    display::Shape shape = down_.Shape();
    down_box_ = down_text_offset_.Copy();
    down_box_.x(down_box_.x() - .5f * shape.first);
    down_box_.y(down_box_.y());
    down_icon_box_ = Update(down_current_, down_offset_);
  }

  left_choice_ = false;
  if(left != "")
  {
    left_ = display::Texture(left, choice_font_, window_);
    left_choice_ = true;
    display::Shape shape = left_.Shape();
    left_box_ = left_text_offset_.Copy();
    left_box_.x(left_box_.x() - shape.first);
    left_box_.y(left_box_.y() - .5f * shape.second);
    left_icon_box_ = Update(left_current_, left_offset_);
  }

  right_choice_ = false;
  if(right != "")
  {
    right_ = display::Texture(right, choice_font_, window_);
    right_choice_ = true;
    display::Shape shape = right_.Shape();
    right_box_ = right_text_offset_.Copy();
    right_box_.x(right_box_.x());
    right_box_.y(right_box_.y() - .5f * shape.second);
    right_icon_box_ = Update(right_current_, right_offset_);
  }
}

void SubtitleImpl::Change(display::BoundingBox& box, State& current, State& next, display::BoundingBox& offset)
{
  if(!(current == next))
  {
    current.Stop();
    current = next;
    current.Play();
    if(paused_)
    {
      current.Pause();
    }
    box = Update(current, offset);
  }
}

display::BoundingBox SubtitleImpl::Update(State& state, display::BoundingBox& offset)
{
  display::BoundingBox temp = state.Shape().Copy();
  temp.x(temp.x() + offset.x());
  temp.y(temp.y() + offset.y());
  return temp;
}

void SubtitleImpl::UpEvent(void)
{
  if(up_choice_ && active_)
  {
    active_ = false;
    up_active_.End([=](){up_signal_(); return false;});
    Change(up_icon_box_, up_current_, up_active_, up_offset_);
    Change(down_icon_box_, down_current_, down_idle_, down_offset_);
    Change(left_icon_box_, left_current_, left_idle_, left_offset_);
    Change(right_icon_box_, right_current_, right_idle_, right_offset_);
  }
}

void SubtitleImpl::DownEvent(void)
{
  if(down_choice_ && active_)
  {
    active_ = false;
    down_active_.End([=](){down_signal_(); return false;});
    Change(up_icon_box_, up_current_, up_idle_, up_offset_);
    Change(down_icon_box_, down_current_, down_active_, down_offset_);
    Change(left_icon_box_, left_current_, left_idle_, left_offset_);
    Change(right_icon_box_, right_current_, right_idle_, right_offset_);
  }
}

void SubtitleImpl::LeftEvent(void)
{
  if(left_choice_ && active_)
  {
    active_ = false;
    left_active_.End([=](){left_signal_(); return false;});
    Change(up_icon_box_, up_current_, up_idle_, up_offset_);
    Change(down_icon_box_, down_current_, down_idle_, down_offset_);
    Change(left_icon_box_, left_current_, left_active_, left_offset_);
    Change(right_icon_box_, right_current_, right_idle_, right_offset_);
  }
}

void SubtitleImpl::RightEvent(void)
{
  if(right_choice_ && active_)
  {
    active_ = false;
    right_active_.End([=](){right_signal_(); return false;});
    Change(up_icon_box_, up_current_, up_idle_, up_offset_);
    Change(down_icon_box_, down_current_, down_idle_, down_offset_);
    Change(left_icon_box_, left_current_, left_idle_, left_offset_);
    Change(right_icon_box_, right_current_, right_active_, right_offset_);
  }
}

void SubtitleImpl::Pause(void)
{
  up_current_.Pause();
  down_current_.Pause();
  left_current_.Pause();
  right_current_.Pause();
  paused_ = true;
}

void SubtitleImpl::Resume(void)
{
  up_current_.Resume();
  down_current_.Resume();
  left_current_.Resume();
  right_current_.Resume();
  paused_ = false;
}

void SubtitleImpl::Up(event::Command const& command)
{
  up_signal_.Add(command);
}

void SubtitleImpl::Down(event::Command const& command)
{
  down_signal_.Add(command);
}

void SubtitleImpl::Left(event::Command const& command)
{
  left_signal_.Add(command);
}

void SubtitleImpl::Right(event::Command const& command)
{
  right_signal_.Add(command);
}

void Subtitle::Text(std::string const& text)
{
  impl_->Text(text);
}

void Subtitle::Up(event::Command const& command)
{
  impl_->Up(command);
}

void Subtitle::Down(event::Command const& command)
{
  impl_->Down(command);
}

void Subtitle::Left(event::Command const& command)
{
  impl_->Left(command);
}

void Subtitle::Right(event::Command const& command)
{
  impl_->Right(command);
}

void Subtitle::Choice(std::string const& up, std::string const& down, std::string const& left, std::string const& right)
{
  impl_->Choice(up, down, left, right);
}

void Subtitle::Pause(void)
{
  impl_->Pause();
}

void Subtitle::Resume(void)
{
  impl_->Resume();
}

void Subtitle::Render(void)
{
  impl_->Render();
}

Subtitle::Subtitle(json::JSON const& json, display::Window& window, event::Queue& queue, event::Event& event)
{
  impl_ = std::make_shared<SubtitleImpl>(json, window, queue);
  impl_->Init(event);
}
}