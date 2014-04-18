#include "subtitle.h"
#include "font.h"
#include "bind.h"
#include "position.h"
#include "state.h"
namespace game
{
class SubtitleImpl final : public std::enable_shared_from_this<SubtitleImpl>
{
public:
  SubtitleImpl(json::JSON const& json, display::Window& window, event::Queue& queue);
  void Render(void);
  void Init(Scene& scene);
  void Text(std::string const& text);
  void Choice(std::string const& up, std::string const& down, std::string const& left, std::string const& right);
  void Clear(void);
  display::Window window_;
  sdl::Font font_;
  display::Texture text_;
  int length_;
  float centre_;
  float text_offset_;
  float icon_offset_;
  bool choice_;
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
};

SubtitleImpl::SubtitleImpl(json::JSON const& json, display::Window& window, event::Queue& queue) : window_(window), choice_(false), centre_(100.f), text_offset_(75.f), icon_offset_(40.f)
{
  json_t* font;
  json_t* up_idle;
  json_t* down_idle;
  json_t* left_idle;
  json_t* right_idle;
  json_t* up_active;
  json_t* down_active;
  json_t* left_active;
  json_t* right_active;
  json.Unpack("{sosisosososososososo}",
    "font", &font,
    "length", &length_,
    "up idle", &up_idle,
    "down idle", &down_idle,
    "left idle", &left_idle,
    "right idle", &right_idle,
    "up active", &up_active,
    "down active", &down_active,
    "left active", &left_active,
    "right active", &right_active);
  font_ = sdl::Font(font);
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
}

void SubtitleImpl::Init(Scene& scene)
{
  scene.Add(event::Bind(&SubtitleImpl::Render, shared_from_this()), std::numeric_limits<int>().max());
}

void SubtitleImpl::Render(void)
{
  if(choice_)
  {
    display::Shape window = window_.Shape();
    display::Shape shape;
    game::Position position;

    shape = up_.Shape();
    position.first = .5f * (window.first - shape.first);
    position.second = window.second - centre_ - text_offset_ - .5f * shape.second;
    up_(display::BoundingBox(), display::BoundingBox(position.first, position.second, 0.f, 0.f), 0.f, false, 0.);

    shape = down_.Shape();
    position.first = .5f * (window.first - shape.first);
    position.second = window.second - centre_ + text_offset_ - .5f * shape.second;
    down_(display::BoundingBox(), display::BoundingBox(position.first, position.second, 0.f, 0.f), 0.f, false, 0.);

    shape = left_.Shape();
    position.first = .5f * window.first - text_offset_ - shape.first;
    position.second = window.second - centre_ - .5f * shape.second;
    left_(display::BoundingBox(), display::BoundingBox(position.first, position.second, 0.f, 0.f), 0.f, false, 0.);

    shape = right_.Shape();
    position.first = .5f * window.first + text_offset_;
    position.second = window.second - centre_ - .5f * shape.second;
    right_(display::BoundingBox(), display::BoundingBox(position.first, position.second, 0.f, 0.f), 0.f, false, 0.);

    display::BoundingBox temp;
    
    temp = up_current_.Shape().Copy();
    temp.x(temp.x() + .5f * window.first);
    temp.y(temp.y() + window.second - centre_ - icon_offset_);
    up_current_.Render(temp, 0.f, false, 0.);

    temp = down_current_.Shape().Copy();
    temp.x(temp.x() + .5f * window.first);
    temp.y(temp.y() + window.second - centre_ + icon_offset_);
    down_current_.Render(temp, 0.f, false, 0.);

    temp = left_current_.Shape().Copy();
    temp.x(temp.x() + .5f * window.first - icon_offset_);
    temp.y(temp.y() + window.second - centre_);
    left_current_.Render(temp, 0.f, false, 0.);

    temp = right_current_.Shape().Copy();
    temp.x(temp.x() + .5f * window.first + icon_offset_);
    temp.y(temp.y() + window.second - centre_);
    right_current_.Render(temp, 0.f, false, 0.);
  }
  else
  {
    display::Shape window = window_.Shape();
    display::Shape shape = text_.Shape();
    game::Position position;
    position.first = .5f * (window.first - shape.first);
    position.second = window.second - centre_ - .5f * shape.second;
    text_(display::BoundingBox(), display::BoundingBox(position.first, position.second, 0.f, 0.f), 0.f, false, 0.);
  }
}

void SubtitleImpl::Text(std::string const& text)
{
  text_ = display::Texture(text, font_, length_, window_);
  choice_ = false;
}

void SubtitleImpl::Choice(std::string const& up, std::string const& down, std::string const& left, std::string const& right)
{
  up_ = display::Texture(up, font_, window_);
  down_ = display::Texture(down, font_, window_);
  left_ = display::Texture(left, font_, window_);
  right_ = display::Texture(right, font_, window_);
  choice_ = true;
}

void SubtitleImpl::Clear(void)
{
  text_ = display::Texture();
  up_ = display::Texture();
  down_ = display::Texture();
  left_ = display::Texture();
  right_ = display::Texture();
  choice_ = false;
}

void Subtitle::Text(std::string const& text)
{
  impl_->Text(text);
}

void Subtitle::Choice(std::string const& up, std::string const& down, std::string const& left, std::string const& right)
{
  impl_->Choice(up, down, left, right);
}

void Subtitle::Clear(void)
{
  impl_->Clear();
}

Subtitle::Subtitle(json::JSON const& json, display::Window& window, Scene& scene, event::Queue& queue)
{
  impl_ = std::make_shared<SubtitleImpl>(json, window, queue);
  impl_->Init(scene);
}
}