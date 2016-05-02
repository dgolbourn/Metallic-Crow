#include "controller.h"
#include "bind.h"
#include "window.h"
#include "menu.h"
#include "script.h"
#include "signal.h"
#include <vector>
#include "saves.h"
#include <sstream>
#include <algorithm>
#include "sound.h"
namespace
{
enum class State : int
{
  Start,
  Chapter,
  Load,
  Pause,
  Story
};

typedef std::vector<std::string> Strings;
typedef std::vector<boost::filesystem::path> Paths;

auto Chapters(lua::Stack& lua, Strings& names, Paths& files, boost::filesystem::path const& path) -> void
{
  for(int index = 1, end = lua.Size(); index <= end; ++index)
  {
    lua::Guard guard = lua.Field(index);
    std::string name = lua.Field<std::string>("name");
    std::string file = lua.Field<std::string>("file");
    names.emplace_back(name);
    files.emplace_back(path / file);
  }
}

auto ChapterOptions(game::Menu& menu, int progress, Strings const& chapters) -> void
{
  Strings options;
  int max = static_cast<int>(chapters.size()) - 1;
  if(progress > max)
  {
    progress = max;
  }
  if(progress < 0)
  {
    progress = 0;
  }
  for(int i = 0; i <= progress; ++i)
  {
    options.push_back(chapters[i]);
  }
  menu(options);
}

auto LoadOptions(game::Menu& menu, game::Saves& saves, Strings const& chapters) -> void
{
  Strings options;
  for(int i = 0; i < saves.Size(); ++i)
  {    
    int progress = saves.Progress(i);
    int current = saves.Current(i);
    std::string last_played = saves.LastPlayed(i);
    std::stringstream stream;
    if((0 == progress) && (last_played == ""))
    {
      stream << "New";
    }
    else 
    {
      if(current != progress)
      {
        stream << chapters[current] << " / ";
      }
      
      if(static_cast<int>(chapters.size()) == progress)
      {
        stream << "Complete";
      }
      else
      {
        stream << chapters[progress];
      }

      stream << " " << last_played;
    }
    options.push_back(stream.str());
  }
  menu(options);
  menu[saves.LastPlayed()];
}
}

namespace game
{
class Controller::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(lua::Stack& lua, event::Queue& queue, boost::filesystem::path const& path);
  auto Init() -> void;
  auto Control(float x, float y) -> void;
  auto Look(float x, float y) -> void;
  auto ChoiceUp() -> void;
  auto ChoiceDown() -> void;
  auto ChoiceLeft() -> void;
  auto ChoiceRight() -> void;
  auto Select() -> void;
  auto Back() -> void;
  auto Render() -> void;
  auto Add(event::Command const& command) -> void;
  auto PauseContinue() -> void;
  auto PauseMainMenu() -> void;
  auto StartPlay() -> void;
  auto StartChooseChapter() -> void;
  auto StartLoad() -> void;
  auto StartQuit() -> void;
  auto Load(int slot) -> void;
  auto Chapter(int chapter) -> void;
  auto ChapterEnd() -> void;

  event::Queue queue_;
  display::Window window_;
  Menu pause_menu_;
  Menu start_menu_;
  Menu chapter_menu_;
  Menu load_menu_;
  Script pause_script_;
  Script start_script_;
  Script story_script_;
  State state_;
  event::Signal signal_;
  Saves saves_;
  Strings chapter_names_;
  Paths chapter_files_;
  int slot_;
  int chapter_;
  boost::filesystem::path path_;
  float volume_;
  audio::Sound navigate_;
  audio::Sound select_;
  audio::Sound back_;
  int sign_;
};

auto Controller::Impl::Load(int slot) -> void
{
  saves_.Stop();
  slot_ = slot;
  chapter_ = saves_.Current(slot_);
  LoadOptions(load_menu_, saves_, chapter_names_);
  state_ = State::Start;
}

auto Controller::Impl::Chapter(int chapter) -> void
{
  saves_.Stop();
  chapter_ = chapter;
  ChapterOptions(chapter_menu_, saves_.Progress(slot_), chapter_names_);
  state_ = State::Start;
}

Controller::Impl::Impl(lua::Stack& lua, event::Queue& queue, boost::filesystem::path const& path) : state_(State::Start), queue_(queue), path_(path), sign_(0)
{
  {
    lua::Guard guard = lua.Field("window");
    window_ = display::Window(lua);
  }

  {
    lua::Guard guard = lua.Field("menu");
    pause_menu_ = Menu(lua, window_, path_);
  }

  pause_menu_({"Continue", "Main Menu"});
  pause_script_ = Script(path_ / lua.Field<std::string>("pause_menu_script"), window_, queue_, path_, volume_);

  {
    lua::Guard guard = lua.Field("menu");
    start_menu_ = Menu(lua, window_, path_);
  }

  start_menu_({"Play", "Chapters", "Load", "Quit"});
  
  start_script_ = Script(path_ / lua.Field<std::string>("start_menu_script"), window_, queue_, path_, volume_);

  saves_ = Saves(path_ / lua.Field<std::string>("saves"));
  
  slot_ = saves_.LastPlayed();
  chapter_ = saves_.Current(slot_);

  {
    lua::Guard guard = lua.Field("chapters");
    Chapters(lua, chapter_names_, chapter_files_, path_);
  }

  {
    lua::Guard guard = lua.Field("menu");
    chapter_menu_ = Menu(lua, window_, path_);
  }

  ChapterOptions(chapter_menu_, saves_.Progress(slot_), chapter_names_);

  {
    lua::Guard guard = lua.Field("menu");
    load_menu_ = Menu(lua, window_, path_);
  }

  LoadOptions(load_menu_, saves_, chapter_names_);

  volume_ = lua.Field<float>("volume");

  {
    lua::Guard guard = lua.Field("sound_navigate");
    navigate_ = audio::Sound(lua, path_);
  }

  {
    lua::Guard guard = lua.Field("sound_select");
    select_ = audio::Sound(lua, path_);
  }

  {
    lua::Guard guard = lua.Field("sound_back");
    back_ = audio::Sound(lua, path_);
  }
  
  navigate_.Resume();
  select_.Resume();
  back_.Resume();
}

auto Controller::Impl::Init() -> void
{
  queue_.Add(function::Bind(&Impl::Render, shared_from_this()));

  pause_menu_.Add(0, function::Bind(&Impl::PauseContinue, shared_from_this()));
  pause_menu_.Add(1, function::Bind(&Impl::PauseMainMenu, shared_from_this()));

  start_menu_.Add(0, function::Bind(&Impl::StartPlay, shared_from_this()));
  start_menu_.Add(1, function::Bind(&Impl::StartChooseChapter, shared_from_this()));
  start_menu_.Add(2, function::Bind(&Impl::StartLoad, shared_from_this()));
  start_menu_.Add(3, function::Bind(&Impl::StartQuit, shared_from_this()));

  for(int i = 0; i < saves_.Size(); ++i)
  {
    load_menu_.Add(i, function::Bind(&Impl::Load, shared_from_this(), i));
  }

  for(Strings::size_type i = 0; i < chapter_names_.size(); ++i)
  {
    chapter_menu_.Add(i, function::Bind(&Impl::Chapter, shared_from_this(), i));
  }

  start_script_.Resume();
}

auto Controller::Impl::ChapterEnd() -> void
{
  saves_.Current(slot_, saves_.Current(slot_) + 1);
  chapter_ = saves_.Current(slot_);
  saves_.Save();
  if(chapter_ >= int(chapter_files_.size()))
  {
    saves_.Stop();
    story_script_ = Script();
    state_ = State::Start;
  }
  else
  {
    story_script_ = Script(chapter_files_[chapter_], window_, queue_, path_, volume_);
    story_script_.Add(function::Bind(&Impl::ChapterEnd, shared_from_this()));
    story_script_.Resume();
  }
}

auto Controller::Impl::PauseContinue() -> void
{
  state_ = State::Story;
  pause_script_.Pause();
  story_script_.Resume();
}

auto Controller::Impl::PauseMainMenu() -> void
{
  saves_.Stop();
  state_ = State::Start;
  pause_script_.Pause();
  start_script_.Resume();
}

auto Controller::Impl::StartPlay() -> void
{
  state_ = State::Story;
  start_script_.Pause();
  saves_.Current(slot_, chapter_);
  chapter_ = saves_.Current(slot_);
  if(chapter_ >= static_cast<int>(chapter_files_.size()))
  {
    saves_.Current(slot_, 0);
    chapter_ = saves_.Current(slot_);
  }
  saves_.Play(slot_);
  saves_.Save();
  story_script_ = Script(chapter_files_[chapter_], window_, queue_, path_, volume_);
  story_script_.Add(function::Bind(&Impl::ChapterEnd, shared_from_this()));
  story_script_.Resume();
}

auto Controller::Impl::StartChooseChapter() -> void
{
  state_ = State::Chapter;
}

auto Controller::Impl::StartLoad() -> void
{
  state_ = State::Load;
}

auto Controller::Impl::StartQuit() -> void
{
  signal_();
}

auto Controller::Impl::Add(event::Command const& command) -> void
{
  signal_.Add(command);
}

auto Controller::Impl::Control(float x, float y) -> void
{
  int sign = static_cast<int>(0.f < y) - static_cast<int>(y < 0.f);
  bool up = (sign_ <= 0) && (sign > 0);
  bool down = (sign_ >= 0) && (sign < 0);
  sign_ = sign;

  switch(state_)
  {
  case State::Start:
    if(up || down)
    {
      navigate_(volume_);
    }
    if(up)
    {
      start_menu_.Previous();
    }
    if(down)
    {
      start_menu_.Next();
    }
    break;
  case State::Pause:
    if(up || down)
    {
      navigate_(volume_);
    }
    if(up)
    {
      pause_menu_.Previous();
    }
    if(down)
    {
      pause_menu_.Next();
    }
    break;
  case State::Chapter:
    if(up || down)
    {
      navigate_(volume_);
    }
    if(up)
    {
      chapter_menu_.Previous();
    }
    if(down)
    {
      chapter_menu_.Next();
    }
    break;
  case State::Load:
    if(up || down)
    {
      navigate_(volume_);
    }
    if(up)
    {
      load_menu_.Previous();
    }
    if(down)
    {
      load_menu_.Next();
    }
    break;
  case State::Story:
    story_script_.Control(x, y);
    break;
  default:
    break;
  }
}

auto Controller::Impl::Look(float x, float y) -> void
{
  switch(state_)
  {
  default:
  case State::Start:
  case State::Pause:
  case State::Chapter:
  case State::Load:
    break;
  case State::Story:
    story_script_.Look(x, y);
    break;
  }
}

auto Controller::Impl::ChoiceUp() -> void
{
  switch(state_)
  {
  default:
  case State::Start:
  case State::Pause:
  case State::Chapter:
  case State::Load:
    break;
  case State::Story:
    story_script_.ChoiceUp();
    break;
  }
}

auto Controller::Impl::ChoiceDown() -> void
{
  switch(state_)
  {
  default:
    break;
  case State::Start:
    select_(volume_);
    start_menu_.Select();
    break;
  case State::Pause:
    select_(volume_);
    pause_menu_.Select();
    break;
  case State::Chapter:
    select_(volume_);
    chapter_menu_.Select();
    break;
  case State::Load:
    select_(volume_);
    load_menu_.Select();
    break;
  case State::Story:
    story_script_.ChoiceDown();
    break;
  }
}

auto Controller::Impl::ChoiceLeft() -> void
{
  switch(state_)
  {
  default:
  case State::Start:
  case State::Pause:
  case State::Chapter:
  case State::Load:
    break;
  case State::Story:
    story_script_.ChoiceLeft();
    break;
  }
}

auto Controller::Impl::ChoiceRight() -> void
{
  switch(state_)
  {
  default:
  case State::Start:
    break;
  case State::Pause:
    back_(volume_);
    state_ = State::Story;
    story_script_.Resume();
    pause_script_.Pause();
    break;
  case State::Load:
  case State::Chapter:
    back_(volume_);
    state_ = State::Start;
    break;
  case State::Story:
    story_script_.ChoiceRight();
    break;
  }
}

auto Controller::Impl::Select() -> void
{
  switch(state_)
  {
  case State::Start:
    select_(volume_);
    start_menu_.Select();
    break;
  case State::Pause:
    select_(volume_);
    pause_menu_.Select();
    break;
  case State::Chapter:
    select_(volume_);
    chapter_menu_.Select();
    break;
  case State::Load:
    select_(volume_);
    load_menu_.Select();
    break;
  default:
  case State::Story:
    back_(volume_);
    state_ = State::Pause;
    story_script_.Pause();
    pause_menu_[0];
    pause_script_.Resume();
    break;
  }
}

auto Controller::Impl::Back() -> void
{
  switch(state_)
  {
  default:
  case State::Start:
    break;
  case State::Pause:
    back_(volume_);
    state_ = State::Story;
    story_script_.Resume();
    pause_script_.Pause();
    break;
  case State::Story:
    back_(volume_);
    state_ = State::Pause;
    story_script_.Pause();
    pause_menu_[0];
    pause_script_.Resume();
    break;
  case State::Load:
  case State::Chapter:
    back_(volume_);
    state_ = State::Start;
    break;
  }
}

auto Controller::Impl::Render() -> void
{
  switch(state_)
  {
  case State::Start:
    window_.Clear();
    start_script_.Render();
    start_menu_.Render();
    window_.Show();
    break;
  case State::Chapter:
    window_.Clear();
    start_script_.Render();
    chapter_menu_.Render();
    window_.Show();
    break;
  case State::Load:
    window_.Clear();
    start_script_.Render();
    load_menu_.Render();
    window_.Show();
    break;
  case State::Pause:
    window_.Clear();
    pause_script_.Render();
    pause_menu_.Render();
    window_.Show();
    break;
  case State::Story:
    window_.Clear();
    story_script_.Render();
    window_.Show();
    break;
  default:
    break;
  }
}

Controller::Controller(lua::Stack& lua, event::Queue& queue, boost::filesystem::path const& path) : impl_(std::make_shared<Impl>(lua, queue, path))
{
  impl_->Init();
}

auto Controller::Control(float x, float y) -> void
{
  impl_->Control(x, y);
}

auto Controller::Look(float x, float y) -> void
{
  impl_->Look(x, y);
}

auto Controller::ChoiceUp() -> void
{
  impl_->ChoiceUp();
}

auto Controller::ChoiceDown() -> void
{
  impl_->ChoiceDown();
}

auto Controller::ChoiceLeft() -> void
{
  impl_->ChoiceLeft();
}

auto Controller::ChoiceRight() -> void
{
  impl_->ChoiceRight();
}

auto Controller::Select() -> void
{
  impl_->Select();
}

auto Controller::Back() -> void
{
  impl_->Back();
}

auto Controller::Add(event::Command const& command) -> void
{
  impl_->Add(command);
}

Controller::operator bool() const
{
  return static_cast<bool>(impl_);
}
}