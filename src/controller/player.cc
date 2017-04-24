#include "player.h"
#include <map>
#include "signal.h"
#include <list>
#include "for_each.h"
#include <set>
#include <algorithm>
namespace
{
struct Texture
{
  display::Texture texture_;
  display::Modulation modulation_;
};
  
struct Drawable
{
  Texture texture_;
  display::BoundingBox render_box_;
};

struct Player
{
  Texture texture_;
  bool action_left_;
  bool action_right_;
};

struct Id
{
  bool active_;
  int player_;
  int sign_;
  float move_x_;
  float move_y_;
  float look_x_;
  float look_y_;
  bool action_left_;
  bool action_right_;
};

typedef std::vector<Drawable> Drawables;
typedef std::vector<Texture> Textures;
typedef std::vector<Player> Players;
typedef std::vector<bool> Table;
typedef std::map<int, Id> Ids;
typedef std::list<game::Player::Control> Controls;
typedef std::list<game::Player::Button> Buttons;
typedef std::list<game::Player::Switch> Switches;
typedef std::list<event::Command> Commands;
}

namespace game
{
class Player::Impl
{
public:
  Impl(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path);

  auto Move(int id, float x, float y) -> void;
  auto Look(int id, float x, float y) -> void;
  auto ChoiceUp(int id) -> void;
  auto ChoiceDown(int id) -> void;
  auto ChoiceLeft(int id) -> void;
  auto ChoiceRight(int id) -> void;
  auto ActionLeft(int id, bool state) -> void;
  auto ActionRight(int id, bool state) -> void;
  auto Select(int id) -> void;
  auto Back(int id) -> void;

  auto Move(Control const& control) -> void;
  auto Look(Control const& control) -> void;
  auto ChoiceUp(Button const& button) -> void;
  auto ChoiceDown(Button const& button) -> void;
  auto ChoiceLeft(Button const& button) -> void;
  auto ChoiceRight(Button const& button) -> void;
  auto ActionLeft(Switch const& zwitch) -> void;
  auto ActionRight(Switch const& zwitch) -> void;

  auto RawUp(Button const& button) -> void;
  auto RawDown(Button const& button) -> void;

  auto AllUp(event::Command const& command) -> void;
  auto AllDown(event::Command const& command) -> void;
  auto AllSelect(event::Command const& command) -> void;
  auto AllBack(event::Command const& command) -> void;
  auto AllChoiceSelect(event::Command const& command) -> void;
  auto AllChoiceBack(event::Command const& command) -> void;

  auto Add(int id) -> void;
  auto Remove(int id) -> void;
  
  auto Up(int id) -> void;
  auto Down(int id) -> void;

  auto Render() -> void;

  auto Join(Switch const& zwitch) -> void;

  auto Current()->std::vector<int>;

  auto Assort() -> void;

  explicit operator bool();

  Drawables drawables_;
  Players players_;
  Textures controllers_;
  Texture inactive_;
  Drawable background_;
  Ids ids_;
  display::Modulation modulation_;
  float row_height_;
  float row_padding_;
  float column_width_;
  float column_padding_;
  Table table_;
  int next_;
  bool redraw_;
  int sign_;

  Controls move_;
  Controls look_;
  Buttons choice_up_;
  Buttons choice_down_;
  Buttons choice_left_;
  Buttons choice_right_;
  Switches action_left_;
  Switches action_right_;
  Switches join_;
  Buttons raw_up_;
  Buttons raw_down_;
  Commands all_up_;
  Commands all_down_;
  Commands all_select_;
  Commands all_back_;
  Commands all_choice_select_;
  Commands all_choice_back_;

  struct Notifier
  {
    std::set<int> prev_;
    Player::Impl& impl_;
    explicit Notifier(Impl& impl);
    auto operator()() -> void;
  };
};

Player::Impl::Notifier::Notifier(Player::Impl& impl) : impl_(impl)
{
  for(auto& id : impl_.ids_)
  {
    if(id.second.active_ && id.second.player_ >= 0)
    {
      prev_.insert(id.second.player_ + 1);
    }
  }
}

auto Player::Impl::Notifier::operator()() -> void
{
  std::set<int> next;
  for(auto& id : impl_.ids_)
  {
    if(id.second.active_ && id.second.player_ >= 0)
    {
      next.insert(id.second.player_ + 1);
    }
  }
  std::vector<int> out(std::max(next.size(), prev_.size()));
  auto end = std::set_difference(prev_.begin(), prev_.end(), next.begin(), next.end(), out.begin());
  for(auto iter = out.begin(); iter != end; ++iter)
  {
    event::for_each(impl_.join_, *iter, false);
  }
  end = std::set_difference(next.begin(), next.end(), prev_.begin(), prev_.end(), out.begin());
  for(auto iter = out.begin(); iter != end; ++iter)
  {
    event::for_each(impl_.join_, *iter, true);
  }
}

auto Player::Impl::Render() -> void
{
  if(redraw_)
  {
    redraw_ = false;
    drawables_.clear();
    drawables_.push_back(background_);

    float dh = row_height_ + row_padding_;
    float dw = column_width_ + column_padding_;

    float h0 = -.5f * players_.size() * (row_height_ + row_padding_);
    float w0 = -.5f * ids_.size() * (column_width_ + column_padding_);

    Drawable inactive;
    inactive.texture_ = inactive_;
    auto shape = inactive.texture_.texture_.Shape();
    float scale = std::min(column_width_ / shape.first, row_height_ / shape.second);
    float w = shape.first * scale;
    float h = shape.second * scale;
    inactive.render_box_ = display::BoundingBox(w0 - w * .5f, h0 - h * .5f, w, h);
    drawables_.push_back(inactive);

    int j = 1;
    for(auto& p : players_)
    {
      Drawable player;
      player.texture_ = p.texture_;
      auto shape = player.texture_.texture_.Shape();
      float scale = std::min(column_width_ / shape.first, row_height_ / shape.second);
      float w = shape.first * scale;
      float h = shape.second * scale;
      player.render_box_ = display::BoundingBox(w0 - w * .5f, h0 - h * .5f + dh * j, w, h);
      drawables_.push_back(player);
      ++j;
    }

    int i = 1;
    for(auto& id : ids_)
    {
      Drawable controller;
      controller.texture_ = controllers_[id.first % controllers_.size()];
      if(id.second.player_ == -1 || !id.second.active_)
      {
        controller.texture_.modulation_ = display::Modulation(
          controller.texture_.modulation_.r() * modulation_.r(),
          controller.texture_.modulation_.g() * modulation_.g(),
          controller.texture_.modulation_.b() * modulation_.b(),
          controller.texture_.modulation_.a() * modulation_.a());
      }
      auto shape = controller.texture_.texture_.Shape();
      float scale = std::min(column_width_ / shape.first, row_height_ / shape.second);
      float w = shape.first * scale;
      float h = shape.second * scale;
      controller.render_box_ = display::BoundingBox(w0 - w * .5f + dw * i, h0 - h * .5f + dh * (id.second.player_ + 1), w, h);
      drawables_.push_back(controller);
      ++i;
    }
  }

  for(auto& drawable : drawables_)
  {
    drawable.texture_.texture_(display::BoundingBox(), drawable.render_box_, 0.f, false, false, 0., drawable.texture_.modulation_);
  }
}

auto Player::Impl::Assort() -> void
{
  if(std::none_of(ids_.cbegin(), ids_.cend(), [](Ids::value_type const& id)
  {
    return id.second.active_ && id.second.player_ >= 0;
  }))
  {
    for(auto& id : ids_)
    {
      if(id.second.active_)
      {
        id.second.player_ = next_;
        ++next_ %= players_.size();
      }
    }
  }
}

auto Player::Impl::Up(int id) -> void
{
  Notifier notifier(*this);
  auto iter = ids_.find(id);
  if(iter != ids_.end() && iter->second.active_)
  {
    if(iter->second.player_ >= 0)
    {
      --iter->second.player_;
      if(iter->second.player_ < 0)
      {
        Assort();
      }
      redraw_ = true;
    }
  }
  notifier();
}

auto Player::Impl::Down(int id) -> void
{
  Notifier notifier(*this);
  auto iter = ids_.find(id);
  if(iter != ids_.end() && iter->second.active_)
  {
    if(iter->second.player_ < (int)players_.size() - 1)
    {
      ++iter->second.player_;
      redraw_ = true;
    }
  }
  notifier();
}

auto Player::Impl::Add(int id) -> void
{
  Notifier notifier(*this);
  auto iter = ids_.find(id);
  if(iter != ids_.end())
  {
    if(!iter->second.active_)
    {
      iter->second.active_ = true;
      Assort();
      redraw_ = true;
    }
  }
  else
  {
    ids_[id] = { true, next_ };
    ++next_ %= players_.size();
    redraw_ = true;
  }
  notifier();
}

auto Player::Impl::Remove(int id) -> void
{
  Notifier notifier(*this);
  auto iter = ids_.find(id);
  if(iter != ids_.end())
  {
    if(iter->second.active_)
    {
      iter->second.active_ = false;
      redraw_ = true;
      Assort();
    }
  }
  notifier();
}

Player::Impl::Impl(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path) : next_(0), redraw_(true), sign_(0)
{
  {
    lua::Guard guard = lua.Field("players");
    for(int index = 1, end = lua.Size(); index <= end; ++index)
    {
      lua::Guard guard = lua.Field(index);
      {
        ::Player player;
        player.action_left_ = false;
        player.action_right_ = false;
        {
          lua::Guard guard = lua.Field("modulation");
          player.texture_.modulation_ = display::Modulation(lua);
        }
        display::BoundingBox clip;
        {
          lua::Guard guard = lua.Field("clip");
          clip = display::BoundingBox(lua);
        }
        player.texture_.texture_ = display::Texture(path / lua.Field<std::string>("page"), window, clip);
        players_.emplace_back(std::move(player));
      }
    }
  }

  {
    lua::Guard guard = lua.Field("inactive");
    {
      Texture player;
      {
        lua::Guard guard = lua.Field("modulation");
        player.modulation_ = display::Modulation(lua);
      }
      display::BoundingBox clip;
      {
        lua::Guard guard = lua.Field("clip");
        clip = display::BoundingBox(lua);
      }
      player.texture_ = display::Texture(path / lua.Field<std::string>("page"), window, clip);
      inactive_ = player;
    }
  }

  {
    lua::Guard guard = lua.Field("controllers");
    controllers_.reserve(lua.Size());
    for(int index = 1, end = lua.Size(); index <= end; ++index)
    {
      lua::Guard guard = lua.Field(index);
      {
        Texture controller;
        {
          lua::Guard guard = lua.Field("modulation");
          controller.modulation_ = display::Modulation(lua);
        }
        display::BoundingBox clip;
        {
          lua::Guard guard = lua.Field("clip");
          clip = display::BoundingBox(lua);
        }
        controller.texture_ = display::Texture(path / lua.Field<std::string>("page"), window, clip);
        controllers_.emplace_back(std::move(controller));
      }
    }
  }

  {
    lua::Guard guard = lua.Field("inactive_modulation");
    modulation_ = display::Modulation(lua);
  }

  {
    lua::Guard guard = lua.Field("background");
    {
      {
        lua::Guard guard = lua.Field("modulation");
        background_.texture_.modulation_ = display::Modulation(lua);
      }
      {
        lua::Guard guard = lua.Field("render_box");
        background_.render_box_ = display::BoundingBox(lua);
      }
      display::BoundingBox clip;
      {
        lua::Guard guard = lua.Field("clip");
        clip = display::BoundingBox(lua);
      }
      background_.texture_.texture_ = display::Texture(path / lua.Field<std::string>("page"), window, clip);
    }
  }

  row_height_ = lua.Field<float>("row_height");
  row_padding_ = lua.Field<float>("row_padding");
  column_width_ = lua.Field<float>("column_width");
  column_padding_ = lua.Field<float>("column_padding");
}

auto Player::Impl::Move(int id, float x, float y) -> void
{ 
  auto iter = ids_.find(id);
  if(iter != ids_.end())
  {
    iter->second.move_x_ = x;
    iter->second.move_y_ = y;

    int sign = static_cast<int>(0.f < y) - static_cast<int>(y < 0.f);
    bool up = (iter->second.sign_ <= 0) && (sign > 0);
    bool down = (iter->second.sign_ >= 0) && (sign < 0);
    iter->second.sign_ = sign;    
    if(up)
    {
      event::for_each(raw_up_, id);
    }
    if(down)
    {
      event::for_each(raw_down_, id);
    }

    if(iter->second.active_ && iter->second.player_ >= 0)
    {
      int all_n = 0;
      float all_y = 0.f;
      int player_n = 0;
      float player_x = 0.f;
      float player_y = 0.f;
      for(auto& id : ids_)
      {
        if(id.second.active_ && id.second.player_ >= 0)
        {
          ++all_n;
          all_y += id.second.move_y_;
          if(id.second.player_ == iter->second.player_)
          {
            ++player_n;
            player_x += id.second.move_x_;
            player_y += id.second.move_y_;
          }
        }
      }

      int sign = static_cast<int>(0.f < all_y) - static_cast<int>(all_y < 0.f);
      bool up = (sign_ <= 0) && (sign > 0);
      bool down = (sign_ >= 0) && (sign < 0);
      sign_ = sign;    
      if(up)
      {
        event::for_each(all_up_);
      }
      if(down)
      {
        event::for_each(all_down_);
      }

      event::for_each(move_, iter->second.player_ + 1, player_x / player_n, player_y / player_n);
    }
  }
}

auto Player::Impl::Look(int id, float x, float y) -> void
{
  auto iter = ids_.find(id);
  if(iter != ids_.end())
  {
    iter->second.look_x_ = x;
    iter->second.look_y_ = y;
    if(iter->second.active_ && iter->second.player_ >= 0)
    {
      int player_n = 0;
      float player_x = 0.f;
      float player_y = 0.f;
      for(auto& id : ids_)
      {
        if(id.second.active_ && id.second.player_ >= 0)
        {
          if(id.second.player_ == iter->second.player_)
          {
            ++player_n;
            player_x += id.second.look_x_;
            player_y += id.second.look_y_;
          }
        }
      }
      event::for_each(look_, iter->second.player_ + 1, player_x / player_n, player_y / player_n);
    }
  }
}

auto Player::Impl::ChoiceUp(int id) -> void
{
  auto iter = ids_.find(id);
  if(iter != ids_.end() && iter->second.active_ && iter->second.player_ >= 0)
  {
    event::for_each(choice_up_, iter->second.player_ + 1);
  }
}

auto Player::Impl::ChoiceDown(int id) -> void
{
  auto iter = ids_.find(id);
  if(iter != ids_.end() && iter->second.active_ && iter->second.player_ >= 0)
  {
    event::for_each(choice_down_, iter->second.player_ + 1);
    event::for_each(all_choice_select_);
  }
}

auto Player::Impl::ChoiceLeft(int id) -> void
{
  auto iter = ids_.find(id);
  if(iter != ids_.end() && iter->second.active_ && iter->second.player_ >= 0)
  {
    event::for_each(choice_left_, iter->second.player_ + 1);
    event::for_each(all_choice_back_);
  }
}

auto Player::Impl::ChoiceRight(int id) -> void
{
  auto iter = ids_.find(id);
  if(iter != ids_.end() && iter->second.active_ && iter->second.player_ >= 0)
  {
    event::for_each(choice_right_, iter->second.player_ + 1);
  }
}

auto Player::Impl::ActionLeft(int id, bool state) -> void
{
  auto iter = ids_.find(id);
  if(iter != ids_.end())
  {
    iter->second.action_left_ = state;

    if(iter->second.active_ && iter->second.player_ >= 0)
    {
      int sign = 0;
      for(auto& id : ids_)
      {
        if(id.second.active_ && id.second.player_ >= 0)
        {
          if(id.second.player_ == iter->second.player_)
          {
            if(id.second.action_left_)
            {
              ++sign;
            }
            else
            {
              --sign;
            }
          }
        }
      }

      ::Player& player = players_.at(iter->second.player_);
      if(player.action_left_)
      {
        if(sign < 0)
        {
          player.action_left_ = false;
          event::for_each(action_left_, iter->second.player_ + 1, false);
        }
      }
      else
      {
        if(sign > 0)
        {
          player.action_left_ = true;
          event::for_each(action_left_, iter->second.player_ + 1, true);
        }
      }
    }
  }
}

auto Player::Impl::ActionRight(int id, bool state) -> void
{
  auto iter = ids_.find(id);
  if(iter != ids_.end())
  {
    iter->second.action_right_ = state;

    if(iter->second.active_ && iter->second.player_ >= 0)
    {
      int sign = 0;
      for(auto& id : ids_)
      {
        if(id.second.active_ && id.second.player_ >= 0)
        {
          if(id.second.player_ == iter->second.player_)
          {
            if(id.second.action_right_)
            {
              ++sign;
            }
            else
            {
              --sign;
            }
          }
        }
      }
      ::Player& player = players_.at(iter->second.player_);
      if(player.action_right_)
      {
        if(sign < 0)
        {
          player.action_right_ = false;
          event::for_each(action_right_, iter->second.player_ + 1, false);
        }
      }
      else
      {
        if(sign > 0)
        {
          player.action_right_ = true;
          event::for_each(action_right_, iter->second.player_ + 1, true);
        }
      }
    }
  }
}

auto Player::Impl::Select(int id) -> void
{
  auto iter = ids_.find(id);
  if(iter != ids_.end() && iter->second.active_ && iter->second.player_ >= 0)
  {
    event::for_each(all_select_);
  }
}

auto Player::Impl::Back(int id) -> void
{
  auto iter = ids_.find(id);
  if(iter != ids_.end() && iter->second.active_ && iter->second.player_ >= 0)
  {
    event::for_each(all_back_);
  }
}


auto Player::Impl::Move(Control const& control) -> void
{
  move_.push_back(control);
}

auto Player::Impl::Look(Control const& control) -> void
{
  look_.push_back(control);
}

auto Player::Impl::ChoiceUp(Button const& button) -> void
{
  choice_up_.push_back(button);
}

auto Player::Impl::ChoiceDown(Button const& button) -> void
{
  choice_down_.push_back(button);
}

auto Player::Impl::ChoiceLeft(Button const& button) -> void
{
  choice_left_.push_back(button);
}

auto Player::Impl::ChoiceRight(Button const& button) -> void
{
  choice_right_.push_back(button);
}

auto Player::Impl::ActionLeft(Switch const& zwitch) -> void
{
  action_left_.push_back(zwitch);
}

auto Player::Impl::ActionRight(Switch const& zwitch) -> void
{
  action_right_.push_back(zwitch);
}

auto Player::Impl::RawUp(Button const& button) -> void
{
  raw_up_.push_back(button);
}

auto Player::Impl::RawDown(Button const& button) -> void
{
  raw_down_.push_back(button);
}

auto Player::Impl::AllUp(event::Command const& command) -> void
{
  all_up_.push_back(command);
}

auto Player::Impl::AllDown(event::Command const& command) -> void
{
  all_down_.push_back(command);
}

auto Player::Impl::AllSelect(event::Command const& command) -> void
{
  all_select_.push_back(command);
}

auto Player::Impl::AllBack(event::Command const& command) -> void
{
  all_back_.push_back(command);
}

auto Player::Impl::AllChoiceSelect(event::Command const& command) -> void
{
  all_choice_select_.push_back(command);
}

auto Player::Impl::AllChoiceBack(event::Command const& command) -> void
{
  all_choice_back_.push_back(command);
}

auto Player::Impl::Current()->std::vector<int>
{
  std::set<int> players;
  for(auto& id : ids_)
  {
    if(id.second.active_ && id.second.player_ >= 0)
    {
      players.insert(id.second.player_ + 1);
    }
  }
  std::vector<int> ret(players.size());
  for(auto player : players)
  {
    ret.push_back(player);
  }
  return ret;
}

auto Player::Impl::Join(Switch const& zwitch) -> void
{
  join_.push_back(zwitch);
}

Player::Impl::operator bool()
{
  int i = 0;
  for(auto& id : ids_)
  {
    i += id.second.active_;
  }
  return i >= 2;
}

Player::Player(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path) : impl_(std::make_shared<Impl>(lua, window, path))
{
}

auto Player::Move(int id, float x, float y) -> void
{
  impl_->Move(id, x, y);
}

auto Player::Look(int id, float x, float y) -> void
{
  impl_->Look(id, x, y);
}

auto Player::ChoiceUp(int id) -> void
{
  impl_->ChoiceUp(id);
}

auto Player::ChoiceDown(int id) -> void
{
  impl_->ChoiceDown(id);
}

auto Player::ChoiceLeft(int id) -> void
{
  impl_->ChoiceLeft(id);
}

auto Player::ChoiceRight(int id) -> void
{
  impl_->ChoiceRight(id);
}

auto Player::ActionLeft(int id, bool state) -> void
{
  impl_->ActionLeft(id, state);
}
  
auto Player::ActionRight(int id, bool state) -> void
{
  impl_->ActionRight(id, state);
}

auto Player::Select(int id) -> void
{
  impl_->Select(id);
}

auto Player::Back(int id) -> void
{
  impl_->Back(id);
}

auto Player::Move(Control const& control) -> void
{
  impl_->Move(control);
}

auto Player::Look(Control const& control) -> void
{
  impl_->Look(control);
}

auto Player::ChoiceUp(Button const& button) -> void
{
  impl_->ChoiceUp(button);
}
  
auto Player::ChoiceDown(Button const& button) -> void
{
  impl_->ChoiceDown(button);
}

auto Player::ChoiceLeft(Button const& button) -> void
{
  impl_->ChoiceLeft(button);
}
  
auto Player::ChoiceRight(Button const& button) -> void
{
  impl_->ChoiceRight(button);
}

auto Player::ActionLeft(Switch const& zwitch) -> void
{
  impl_->ActionLeft(zwitch);
}

auto Player::ActionRight(Switch const& zwitch) -> void
{
  impl_->ActionRight(zwitch);
}

auto Player::RawUp(Button const& button) -> void
{
  impl_->RawUp(button);
}

auto Player::RawDown(Button const& button) -> void
{
  impl_->RawDown(button);
}

auto Player::AllUp(event::Command const& command) -> void
{
  impl_->AllUp(command);
}

auto Player::AllDown(event::Command const& command) -> void
{
  impl_->AllDown(command);
}
  
auto Player::AllSelect(event::Command const& command) -> void
{
  impl_->AllSelect(command);
}

auto Player::AllBack(event::Command const& command) -> void
{
  impl_->AllBack(command);
}

auto Player::AllChoiceSelect(event::Command const& command) -> void
{
  impl_->AllChoiceSelect(command);
}

auto Player::AllChoiceBack(event::Command const& command) -> void
{
  impl_->AllChoiceBack(command);
}

auto Player::Add(int id) -> void
{
  impl_->Add(id);
}

auto Player::Remove(int id) -> void
{
  impl_->Remove(id);
}

auto Player::Up(int id) -> void
{
  impl_->Up(id);
}

auto Player::Down(int id) -> void
{
  impl_->Down(id);
}

auto Player::Render() -> void
{
  impl_->Render();
}

auto Player::Join(Switch const& zwitch) -> void
{
  impl_->Join(zwitch);
}

auto Player::Current()->std::vector<int>
{
  return impl_->Current();
}

Player::operator bool()
{
  return impl_ && *impl_;
}
}