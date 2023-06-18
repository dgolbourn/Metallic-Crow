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
#include "player.h"
namespace
{
enum class State : int
{
  Start,
  Chapter,
  Load,
  Pause,
  Delete,
  Story,
  Player,
  PausePlayer
};

typedef std::vector<std::string> Strings;
typedef std::vector<boost::filesystem::path> Paths;

auto FirstStart(game::Saves const& saves) -> bool
{
  for(int i = 0; i < saves.Size(); ++i)
  {
    if(0 != saves.Progress(i))
    {
      return false;
    }
  }
  return true;
}

auto NewStart(game::Saves const& saves) -> bool
{
  return 0 == saves.Progress(saves.LastPlayed());
}

auto CompleteStart(game::Saves const& saves, int complete) -> bool
{
  return complete == saves.Current(saves.LastPlayed());
}

auto FullStart(game::Saves const& saves) -> bool
{
  for(int i = 0; i < saves.Size(); ++i)
  {
    if((0 == saves.Progress(i)))
    {
      return false;
    }
  }
  return true;
}

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

auto FirstStartOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({ "Begin", false});
  options.push_back({ "Controllers", false});
  options.push_back({ "Quit to Desktop", false});
  menu(options);
}

auto ContinueStartOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({ "Continue", false});
  options.push_back({ "Continue From Chapter", false});
  options.push_back({ "Start Again (current progress will be lost)", false });
  options.push_back({ "Load Saved Game / Start New Game", false});
  options.push_back({ "Controllers", false});
  options.push_back({ "Quit to Desktop", false});
  menu(options);
}

auto StartFullOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({ "Continue", false});
  options.push_back({ "Continue From Chapter", false});
  options.push_back({ "Start Again (current progress will be lost)", false });
  options.push_back({ "Load Saved Game", false});
  options.push_back({ "Controllers", false});
  options.push_back({ "Quit to Desktop", false});
  menu(options);
}

auto CompleteStartOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({ "Continue From Chapter", false});
  options.push_back({ "Start Again (current progress will be lost)", false });
  options.push_back({ "Load Saved Game / Start New Game", false});
  options.push_back({ "Controllers", false});
  options.push_back({ "Quit to Desktop", false});
  menu(options);
}

auto CompleteStartFullOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({ "Continue From Chapter", false});
  options.push_back({ "Start Again (current progress will be lost)", false });
  options.push_back({ "Load Saved Game", false});
  options.push_back({ "Controllers", false});
  options.push_back({ "Quit to Desktop", false});
  menu(options);
}

auto NewStartOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({ "Begin", false});
  options.push_back({ "Load Saved Game / Start New Game", false});
  options.push_back({ "Controllers", false});
  options.push_back({ "Quit to Desktop", false});
  menu(options);
}

auto NewStartFullOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({ "Begin", false});
  options.push_back({ "Load Saved Game", false});
  options.push_back({ "Controllers", false});
  options.push_back({ "Quit to Desktop", false});
  menu(options);
}

auto DeleteOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({"Proceed (delete this save)", false});
  options.push_back({"No! Keep this Save", false});
  menu(options);
}

auto OneFirstStartOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({ "Begin", false});
  options.push_back({ "Quit to Desktop", false});
  menu(options);
}

auto OneContinueStartOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({ "Continue", false});
  options.push_back({ "Continue From Chapter", false});
  options.push_back({ "Start Again (current progress will be lost)", false });
  options.push_back({ "Load Saved Game / Start New Game", false});
  options.push_back({ "Quit to Desktop", false});
  menu(options);
}

auto OneStartFullOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({ "Continue", false});
  options.push_back({ "Continue From Chapter", false});
  options.push_back({ "Start Again (current progress will be lost)", false });
  options.push_back({ "Load Saved Game", false});
  options.push_back({ "Quit to Desktop", false});
  menu(options);
}

auto OneCompleteStartOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({ "Continue From Chapter", false});
  options.push_back({ "Start Again (current progress will be lost)", false });
  options.push_back({ "Load Saved Game / Start New Game", false});
  options.push_back({ "Quit to Desktop", false});
  menu(options);
}

auto OneCompleteStartFullOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({ "Continue From Chapter", false});
  options.push_back({ "Start Again (current progress will be lost)", false });
  options.push_back({ "Load Saved Game", false});
  options.push_back({ "Quit to Desktop", false});
  menu(options);
}

auto OneNewStartOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({ "Begin", false});
  options.push_back({ "Load Saved Game / Start New Game", false});
  options.push_back({ "Quit to Desktop", false});
  menu(options);
}

auto OneNewStartFullOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({ "Begin", false});
  options.push_back({ "Load Saved Game", false});
  options.push_back({ "Quit to Desktop", false});
  menu(options);
}


auto ChapterOptions(game::Menu& menu, int progress, Strings const& chapters) -> void
{
  game::Menu::Options options;
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
    options.push_back({chapters[i], false});
  }
  menu(options);
}

auto LoadOptions(game::Menu& menu, game::Saves& saves, Strings const& chapters) -> void
{
  game::Menu::Options options;
  for(int i = 0; i < saves.Size(); ++i)
  {    
    int progress = saves.Progress(i);
    int current = saves.Current(i);
    std::string last_played = saves.LastPlayed(i);
    std::stringstream stream;
    if(0 == progress)
    {
      stream << "New profile " << i;
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
    options.push_back({ stream.str(), false });
  }
  menu(options);
  menu[saves.LastPlayed()];
}

auto PlayerPauseMenuOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({"Resume Story", false});
  options.push_back({"Controllers", false});
  options.push_back({"Quit to Menu", false});
  options.push_back({"Quit to Desktop", false});
  menu(options);
}

auto OnePauseMenuOptions(game::Menu& menu) -> void
{
  game::Menu::Options options;
  options.push_back({"Resume Story", false});
  options.push_back({"Quit to Menu", false});
  options.push_back({"Quit to Desktop", false});
  menu(options);
}
}

namespace game
{
class Controller::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(lua::Stack& lua, event::Queue& queue, boost::filesystem::path const& path);
  auto Init() -> void;

  auto MoveEvent(int player, float x, float y) -> void;
  auto LookEvent(int player, float x, float y) -> void;
  auto ChoiceUpEvent(int player) -> void;
  auto ChoiceDownEvent(int player) -> void;
  auto ChoiceLeftEvent(int player) -> void;
  auto ChoiceRightEvent(int player) -> void;
  auto ActionLeftEvent(int player, bool state) -> void;
  auto ActionRightEvent(int player, bool state) -> void;
  auto RawUpEvent(int id) -> void;
  auto RawDownEvent(int id) -> void;
  auto AllUpEvent() -> void;
  auto AllDownEvent() -> void;
  auto AllSelectEvent() -> void;
  auto AllBackEvent() -> void;
  auto AllChoiceSelectEvent() -> void;
  auto AllChoiceBackEvent() -> void;
  auto JoinEvent(int player, bool state) -> void;

  auto Render() -> void;
  auto Quit(event::Command const& command) -> void;
  auto PauseContinue() -> void;
  auto PauseMainMenu() -> void;
  auto PauseQuit() -> void;
  auto StartPlay() -> void;
  auto StartChooseChapter() -> void;
  auto StartDeleteSave() -> void;
  auto StartLoad() -> void;
  auto StartQuit() -> void;
  auto DeletePlay() -> void;
  auto DeleteBack() -> void;
  auto Load(int slot) -> void;
  auto Chapter(int chapter) -> void;
  auto ChapterEnd() -> void;
  auto StartMenu() -> void;
  auto Add(int id) -> void;
  auto Remove(int id) -> void;
  auto Player() -> void;
  auto PausePlayer() -> void;
  auto PauseMenu() -> void;

  event::Queue queue_;
  display::Window window_;
  Menu one_pause_menu_;
  Menu player_pause_menu_;
  Menu pause_menu_;
  Menu first_start_menu_;
  Menu continue_start_menu_;
  Menu start_full_menu_;
  Menu new_start_menu_;
  Menu new_start_full_menu_;
  Menu complete_start_menu_;
  Menu complete_start_full_menu_;
  Menu chapter_menu_;
  Menu load_menu_;
  Menu start_menu_;
  Menu delete_menu_;
  Menu one_first_start_menu_;
  Menu one_continue_start_menu_;
  Menu one_start_full_menu_;
  Menu one_new_start_menu_;
  Menu one_new_start_full_menu_;
  Menu one_complete_start_menu_;
  Menu one_complete_start_full_menu_;
  Menu one_chapter_menu_;
  Menu one_load_menu_;
  Menu one_start_menu_;
  Script pause_script_;
  Script start_script_;
  Script story_script_;
  State state_;
  event::Signal signal_;
  Saves saves_;
  Strings chapter_names_;
  Paths chapter_files_;
  boost::filesystem::path path_;
  float volume_;
  audio::Sound navigate_;
  audio::Sound select_;
  audio::Sound back_;
  int sign_;
  game::Player player_;
};

auto Controller::Impl::Player() -> void
{
  state_ = State::Player;
}

auto Controller::Impl::PausePlayer() -> void
{
  state_ = State::PausePlayer;
}

auto Controller::Impl::Load(int slot) -> void
{
  saves_.Play(slot);
  ChapterOptions(chapter_menu_, saves_.Progress(saves_.LastPlayed()), chapter_names_);
  LoadOptions(load_menu_, saves_, chapter_names_);
  StartMenu();
  if(NewStart(saves_))
  {
    StartPlay();
  }
  else
  {
    state_ = State::Start;
  }
}

auto Controller::Impl::Chapter(int chapter) -> void
{
  saves_.Current(saves_.LastPlayed(), chapter);
  ChapterOptions(chapter_menu_, saves_.Progress(saves_.LastPlayed()), chapter_names_);
  LoadOptions(load_menu_, saves_, chapter_names_);
  StartMenu();
  StartPlay();
}

Controller::Impl::Impl(lua::Stack& lua, event::Queue& queue, boost::filesystem::path const& path) : state_(State::Start), queue_(queue), path_(path), sign_(0)
{
  {
    lua::Guard guard = lua.Field("window");
    window_ = display::Window(lua);
  }

  {
    lua::Guard guard = lua.Field("menu");
    one_pause_menu_ = Menu(lua, window_, path_);
  }
  OnePauseMenuOptions(one_pause_menu_);

  {
    lua::Guard guard = lua.Field("menu");
    player_pause_menu_ = Menu(lua, window_, path_);
  }
  PlayerPauseMenuOptions(player_pause_menu_);

  pause_script_ = Script(path_ / lua.Field<std::string>("pause_menu_script"), window_, queue_, path_, volume_);

  {
    lua::Guard guard = lua.Field("menu");
    first_start_menu_ = Menu(lua, window_, path_);
  }
  FirstStartOptions(first_start_menu_);

  {
    lua::Guard guard = lua.Field("menu");
    continue_start_menu_ = Menu(lua, window_, path_);
  }
  ContinueStartOptions(continue_start_menu_);

  {
    lua::Guard guard = lua.Field("menu");
    start_full_menu_ = Menu(lua, window_, path_);
  }
  StartFullOptions(start_full_menu_);

  {
    lua::Guard guard = lua.Field("menu");
    new_start_menu_ = Menu(lua, window_, path_);
  }
  NewStartOptions(new_start_menu_);

  {
    lua::Guard guard = lua.Field("menu");
    new_start_full_menu_ = Menu(lua, window_, path_);
  }
  NewStartFullOptions(new_start_full_menu_);

  {
    lua::Guard guard = lua.Field("menu");
    complete_start_menu_ = Menu(lua, window_, path_);
  }
  CompleteStartOptions(complete_start_menu_);

  {
    lua::Guard guard = lua.Field("menu");
    complete_start_full_menu_ = Menu(lua, window_, path_);
  }
  CompleteStartFullOptions(complete_start_full_menu_);

  start_script_ = Script(path_ / lua.Field<std::string>("start_menu_script"), window_, queue_, path_, volume_);

  saves_ = Saves(path_ / lua.Field<std::string>("saves"));

  {
    lua::Guard guard = lua.Field("chapters");
    Chapters(lua, chapter_names_, chapter_files_, path_);
  }

  {
    lua::Guard guard = lua.Field("menu");
    chapter_menu_ = Menu(lua, window_, path_);
  }

  ChapterOptions(chapter_menu_, saves_.Progress(saves_.LastPlayed()), chapter_names_);

  {
    lua::Guard guard = lua.Field("menu");
    load_menu_ = Menu(lua, window_, path_);
  }

  LoadOptions(load_menu_, saves_, chapter_names_);

  {
    lua::Guard guard = lua.Field("menu");
    delete_menu_ = Menu(lua, window_, path_);
  }

  DeleteOptions(delete_menu_);

  {
    lua::Guard guard = lua.Field("menu");
    one_first_start_menu_ = Menu(lua, window_, path_);
  }
  OneFirstStartOptions(one_first_start_menu_);

  {
    lua::Guard guard = lua.Field("menu");
    one_continue_start_menu_ = Menu(lua, window_, path_);
  }
  OneContinueStartOptions(one_continue_start_menu_);

  {
    lua::Guard guard = lua.Field("menu");
    one_start_full_menu_ = Menu(lua, window_, path_);
  }
  OneStartFullOptions(one_start_full_menu_);

  {
    lua::Guard guard = lua.Field("menu");
    one_new_start_menu_ = Menu(lua, window_, path_);
  }
  OneNewStartOptions(one_new_start_menu_);

  {
    lua::Guard guard = lua.Field("menu");
    one_new_start_full_menu_ = Menu(lua, window_, path_);
  }
  OneNewStartFullOptions(one_new_start_full_menu_);

  {
    lua::Guard guard = lua.Field("menu");
    one_complete_start_menu_ = Menu(lua, window_, path_);
  }
  OneCompleteStartOptions(one_complete_start_menu_);

  {
    lua::Guard guard = lua.Field("menu");
    one_complete_start_full_menu_ = Menu(lua, window_, path_);
  }
  OneCompleteStartFullOptions(one_complete_start_full_menu_);

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

  {
    lua::Guard guard = lua.Field("player");
    player_ = game::Player(lua, window_, path_);
  }

  StartMenu();
  PauseMenu();

  navigate_.Resume();
  select_.Resume();
  back_.Resume();
}

auto Controller::Impl::PauseMenu() -> void
{
  if(player_)
  {
    pause_menu_ = player_pause_menu_;
  }
  else
  {
    pause_menu_ = one_pause_menu_;
  }
}

auto Controller::Impl::StartMenu() -> void
{
  if(FirstStart(saves_))
  {
    if(player_)
    {
      start_menu_ = first_start_menu_;
    }
    else
    {
      start_menu_ = one_first_start_menu_;
    }
  }
  else
  {
    bool new_start = NewStart(saves_);
    bool full_start = FullStart(saves_);
    bool complete_start = CompleteStart(saves_, static_cast<int>(chapter_files_.size()));
    if(new_start)
    {
      if(full_start)
      {
        if(player_)
        {
          start_menu_ = new_start_full_menu_;
        }
        else
        {
          start_menu_ = one_new_start_full_menu_;
        }
      }
      else
      {
        if(player_)
        {
          start_menu_ = new_start_menu_;
        }
        else
        {
          start_menu_ = one_new_start_menu_;
        }
      }
    }
    else if(complete_start)
    {
      if(full_start)
      {
        if(player_)
        {
          start_menu_ = complete_start_full_menu_;
        }
        else
        {
          start_menu_ = one_complete_start_full_menu_;
        }
      }
      else
      {
        if(player_)
        {
          start_menu_ = complete_start_menu_;
        }
        else
        {
          start_menu_ = one_complete_start_menu_;
        }
      }
    }
    else
    {
      if(full_start)
      {
        if(player_)
        {
          start_menu_ = start_full_menu_;
        }
        else
        {
          start_menu_ = one_start_full_menu_;
        }
      }
      else
      {
        if(player_)
        {
          start_menu_ = continue_start_menu_;
        }
        else
        {
          start_menu_ = one_continue_start_menu_;
        }
      }
    }
  }
}

auto Controller::Impl::Init() -> void
{
  queue_.Add(function::Bind(&Impl::Render, shared_from_this()));

  one_pause_menu_.Add(0, function::Bind(&Impl::PauseContinue, shared_from_this()));
  one_pause_menu_.Add(1, function::Bind(&Impl::PauseMainMenu, shared_from_this()));
  one_pause_menu_.Add(2, function::Bind(&Impl::PauseQuit, shared_from_this()));

  player_pause_menu_.Add(0, function::Bind(&Impl::PauseContinue, shared_from_this()));
  player_pause_menu_.Add(1, function::Bind(&Impl::PausePlayer, shared_from_this()));
  player_pause_menu_.Add(2, function::Bind(&Impl::PauseMainMenu, shared_from_this()));
  player_pause_menu_.Add(3, function::Bind(&Impl::PauseQuit, shared_from_this()));

  first_start_menu_.Add(0, function::Bind(&Impl::StartPlay, shared_from_this()));
  first_start_menu_.Add(1, function::Bind(&Impl::Player, shared_from_this()));
  first_start_menu_.Add(2, function::Bind(&Impl::StartQuit, shared_from_this()));

  continue_start_menu_.Add(0, function::Bind(&Impl::StartPlay, shared_from_this()));
  continue_start_menu_.Add(1, function::Bind(&Impl::StartChooseChapter, shared_from_this()));
  continue_start_menu_.Add(2, function::Bind(&Impl::StartDeleteSave, shared_from_this()));
  continue_start_menu_.Add(3, function::Bind(&Impl::StartLoad, shared_from_this()));
  continue_start_menu_.Add(4, function::Bind(&Impl::Player, shared_from_this()));
  continue_start_menu_.Add(5, function::Bind(&Impl::StartQuit, shared_from_this()));

  start_full_menu_.Add(0, function::Bind(&Impl::StartPlay, shared_from_this()));
  start_full_menu_.Add(1, function::Bind(&Impl::StartChooseChapter, shared_from_this()));
  start_full_menu_.Add(2, function::Bind(&Impl::StartDeleteSave, shared_from_this()));
  start_full_menu_.Add(3, function::Bind(&Impl::StartLoad, shared_from_this()));
  start_full_menu_.Add(4, function::Bind(&Impl::Player, shared_from_this()));
  start_full_menu_.Add(5, function::Bind(&Impl::StartQuit, shared_from_this()));

  complete_start_menu_.Add(0, function::Bind(&Impl::StartChooseChapter, shared_from_this()));
  complete_start_menu_.Add(1, function::Bind(&Impl::StartDeleteSave, shared_from_this()));
  complete_start_menu_.Add(2, function::Bind(&Impl::StartLoad, shared_from_this()));
  complete_start_menu_.Add(3, function::Bind(&Impl::Player, shared_from_this()));
  complete_start_menu_.Add(4, function::Bind(&Impl::StartQuit, shared_from_this()));

  complete_start_full_menu_.Add(0, function::Bind(&Impl::StartChooseChapter, shared_from_this()));
  complete_start_full_menu_.Add(1, function::Bind(&Impl::StartDeleteSave, shared_from_this()));
  complete_start_full_menu_.Add(2, function::Bind(&Impl::StartLoad, shared_from_this()));
  complete_start_full_menu_.Add(3, function::Bind(&Impl::Player, shared_from_this()));
  complete_start_full_menu_.Add(4, function::Bind(&Impl::StartQuit, shared_from_this()));

  new_start_menu_.Add(0, function::Bind(&Impl::StartPlay, shared_from_this()));
  new_start_menu_.Add(1, function::Bind(&Impl::StartLoad, shared_from_this()));
  new_start_menu_.Add(2, function::Bind(&Impl::Player, shared_from_this()));
  new_start_menu_.Add(3, function::Bind(&Impl::StartQuit, shared_from_this()));

  new_start_full_menu_.Add(0, function::Bind(&Impl::StartPlay, shared_from_this()));
  new_start_full_menu_.Add(1, function::Bind(&Impl::StartLoad, shared_from_this()));
  new_start_full_menu_.Add(2, function::Bind(&Impl::Player, shared_from_this()));
  new_start_full_menu_.Add(3, function::Bind(&Impl::StartQuit, shared_from_this()));

  delete_menu_.Add(0, function::Bind(&Impl::DeletePlay, shared_from_this()));
  delete_menu_.Add(1, function::Bind(&Impl::DeleteBack, shared_from_this()));

  one_first_start_menu_.Add(0, function::Bind(&Impl::StartPlay, shared_from_this()));
  one_first_start_menu_.Add(1, function::Bind(&Impl::StartQuit, shared_from_this()));

  one_continue_start_menu_.Add(0, function::Bind(&Impl::StartPlay, shared_from_this()));
  one_continue_start_menu_.Add(1, function::Bind(&Impl::StartChooseChapter, shared_from_this()));
  one_continue_start_menu_.Add(2, function::Bind(&Impl::StartDeleteSave, shared_from_this()));
  one_continue_start_menu_.Add(3, function::Bind(&Impl::StartLoad, shared_from_this()));
  one_continue_start_menu_.Add(4, function::Bind(&Impl::StartQuit, shared_from_this()));

  one_start_full_menu_.Add(0, function::Bind(&Impl::StartPlay, shared_from_this()));
  one_start_full_menu_.Add(1, function::Bind(&Impl::StartChooseChapter, shared_from_this()));
  one_start_full_menu_.Add(2, function::Bind(&Impl::StartDeleteSave, shared_from_this()));
  one_start_full_menu_.Add(3, function::Bind(&Impl::StartLoad, shared_from_this()));
  one_start_full_menu_.Add(4, function::Bind(&Impl::StartQuit, shared_from_this()));

  one_complete_start_menu_.Add(0, function::Bind(&Impl::StartChooseChapter, shared_from_this()));
  one_complete_start_menu_.Add(1, function::Bind(&Impl::StartDeleteSave, shared_from_this()));
  one_complete_start_menu_.Add(2, function::Bind(&Impl::StartLoad, shared_from_this()));
  one_complete_start_menu_.Add(3, function::Bind(&Impl::StartQuit, shared_from_this()));

  one_complete_start_full_menu_.Add(0, function::Bind(&Impl::StartChooseChapter, shared_from_this()));
  one_complete_start_full_menu_.Add(1, function::Bind(&Impl::StartDeleteSave, shared_from_this()));
  one_complete_start_full_menu_.Add(2, function::Bind(&Impl::StartLoad, shared_from_this()));
  one_complete_start_full_menu_.Add(3, function::Bind(&Impl::StartQuit, shared_from_this()));

  one_new_start_menu_.Add(0, function::Bind(&Impl::StartPlay, shared_from_this()));
  one_new_start_menu_.Add(1, function::Bind(&Impl::StartLoad, shared_from_this()));
  one_new_start_menu_.Add(2, function::Bind(&Impl::StartQuit, shared_from_this()));

  one_new_start_full_menu_.Add(0, function::Bind(&Impl::StartPlay, shared_from_this()));
  one_new_start_full_menu_.Add(1, function::Bind(&Impl::StartLoad, shared_from_this()));
  one_new_start_full_menu_.Add(2, function::Bind(&Impl::StartQuit, shared_from_this()));

  for(int i = 0; i < saves_.Size(); ++i)
  {
    load_menu_.Add(i, function::Bind(&Impl::Load, shared_from_this(), i));
  }

  for(Strings::size_type i = 0; i < chapter_names_.size(); ++i)
  {
    chapter_menu_.Add(static_cast<int>(i), function::Bind(&Impl::Chapter, shared_from_this(), static_cast<int>(i)));
  }

  player_.Move(function::Bind(&Impl::MoveEvent, shared_from_this()));
  player_.Look(function::Bind(&Impl::LookEvent, shared_from_this()));
  player_.ChoiceUp(function::Bind(&Impl::ChoiceUpEvent, shared_from_this()));
  player_.ChoiceDown(function::Bind(&Impl::ChoiceDownEvent, shared_from_this()));
  player_.ChoiceLeft(function::Bind(&Impl::ChoiceLeftEvent, shared_from_this()));
  player_.ChoiceRight(function::Bind(&Impl::ChoiceRightEvent, shared_from_this()));
  player_.ActionLeft(function::Bind(&Impl::ActionLeftEvent, shared_from_this()));
  player_.ActionRight(function::Bind(&Impl::ActionRightEvent, shared_from_this()));

  player_.RawUp(function::Bind(&Impl::RawUpEvent, shared_from_this()));
  player_.RawDown(function::Bind(&Impl::RawDownEvent, shared_from_this()));

  player_.AllUp(function::Bind(&Impl::AllUpEvent, shared_from_this()));
  player_.AllDown(function::Bind(&Impl::AllDownEvent, shared_from_this()));
  player_.AllSelect(function::Bind(&Impl::AllSelectEvent, shared_from_this()));
  player_.AllBack(function::Bind(&Impl::AllBackEvent, shared_from_this()));
  player_.AllChoiceSelect(function::Bind(&Impl::AllChoiceSelectEvent, shared_from_this()));
  player_.AllChoiceBack(function::Bind(&Impl::AllChoiceBackEvent, shared_from_this()));

  player_.Join(function::Bind(&Impl::JoinEvent, shared_from_this()));

  start_script_.Resume();
}

auto Controller::Impl::JoinEvent(int player, bool state) -> void
{
  PauseMenu();
  if(story_script_)
  {
    if(state)
    {
      story_script_.Join(player);
    }
    else
    {
      story_script_.Leave(player);
    }
  }
}

auto Controller::Impl::ChapterEnd() -> void
{
  int chapter = saves_.Current(saves_.LastPlayed()) + 1;
  if(chapter >= static_cast<int>(chapter_files_.size()))
  {
    chapter = static_cast<int>(chapter_files_.size());
    saves_.Stop();
    state_ = State::Start;
    StartMenu();
  }
  story_script_ = Script();
  saves_.Current(saves_.LastPlayed(), chapter);
  saves_.Save();
  StartMenu();
  LoadOptions(load_menu_, saves_, chapter_names_);
  ChapterOptions(chapter_menu_, saves_.Progress(saves_.LastPlayed()), chapter_names_);
}

auto Controller::Impl::DeletePlay() -> void
{
  saves_.Stop();
  saves_.Delete(saves_.LastPlayed());
  StartMenu();
  LoadOptions(load_menu_, saves_, chapter_names_);
  StartPlay();
}

auto Controller::Impl::DeleteBack() -> void
{
  state_ = State::Start;
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
  StartMenu();
  pause_script_.Pause();
  start_script_.Resume();
}

auto Controller::Impl::PauseQuit() -> void
{
  saves_.Stop();
  signal_();
}

auto Controller::Impl::StartDeleteSave() -> void
{
  state_ = State::Delete;
}

auto Controller::Impl::StartPlay() -> void
{
  state_ = State::Story;
  start_script_.Pause();
  if(saves_.LastPlayed() >= static_cast<int>(chapter_files_.size()))
  {
    saves_.Current(saves_.LastPlayed(), 0);
    StartMenu();
  }
  saves_.Play(saves_.LastPlayed());
  story_script_ = Script(chapter_files_[saves_.Current(saves_.LastPlayed())], window_, queue_, path_, volume_);
  story_script_.Add(function::Bind(&Impl::ChapterEnd, shared_from_this()));
  for(auto i : player_.Current())
  {
    story_script_.Join(i);
  }
  story_script_.Resume();
}

auto Controller::Impl::StartChooseChapter() -> void
{
  state_ = State::Chapter;
}

auto Controller::Impl::StartLoad() -> void
{
  state_ = State::Load;
  LoadOptions(load_menu_, saves_, chapter_names_);
  StartMenu();
}

auto Controller::Impl::StartQuit() -> void
{
  signal_();
}

auto Controller::Impl::Quit(event::Command const& command) -> void
{
  signal_.Add(command);
}


auto Controller::Impl::Add(int id) -> void
{
  player_.Add(id);
  StartMenu();
}

auto Controller::Impl::Remove(int id) -> void
{
  player_.Remove(id);
  StartMenu();
}

auto Controller::Impl::MoveEvent(int player, float x, float y) -> void
{
  switch(state_)
  {
  case State::Story:
    story_script_.Move(player, x, y);
    break;
  default:
    break;
  }
}

auto Controller::Impl::LookEvent(int player, float x, float y) -> void
{
  switch(state_)
  {
  case State::Story:
    story_script_.Look(player, x, y);
    break;
  default:
    break;
  }
}

auto Controller::Impl::ChoiceUpEvent(int player) -> void
{
  switch(state_)
  {
  case State::Story:
    story_script_.ChoiceUp(player);
    break;
  default:
    break;
  }
}

auto Controller::Impl::ChoiceDownEvent(int player) -> void
{
  switch(state_)
  {
  case State::Story:
    story_script_.ChoiceDown(player);
    break;
  default:
    break;
  }
}

auto Controller::Impl::ChoiceLeftEvent(int player) -> void
{
  switch(state_)
  {
  case State::Story:
    story_script_.ChoiceLeft(player);
    break;
  default:
    break;
  }
}

auto Controller::Impl::ChoiceRightEvent(int player) -> void
{
  switch(state_)
  {
  case State::Story:
    story_script_.ChoiceRight(player);
    break;
  default:
    break;
  }
}

auto Controller::Impl::ActionLeftEvent(int player, bool state) -> void
{
  switch(state_)
  {
  case State::Story:
    story_script_.ActionLeft(player, state);
    break;
  default:
    break;
  }
}

auto Controller::Impl::ActionRightEvent(int player, bool state) -> void
{
  switch(state_)
  {
  case State::Story:
    story_script_.ActionRight(player, state);
    break;
  default:
    break;
  }
}

auto Controller::Impl::RawUpEvent(int id) -> void
{
  switch(state_)
  {
  case State::PausePlayer:
  case State::Player:
    player_.Up(id);
    break;
  default:
    break;
  }
}

auto Controller::Impl::RawDownEvent(int id) -> void
{
  switch(state_)
  {
  case State::PausePlayer:
  case State::Player:
    player_.Down(id);
    break;
  default:
    break;
  }
}

auto Controller::Impl::AllUpEvent() -> void
{
  switch(state_)
  {
  case State::Start:
    navigate_(volume_);
    start_menu_.Previous();
    break;
  case State::Pause:
    navigate_(volume_);
    pause_menu_.Previous();
    break;
  case State::Chapter:
    navigate_(volume_);
    chapter_menu_.Previous();
    break;
  case State::Load:
    navigate_(volume_);
    load_menu_.Previous();
    break;
  case State::Delete:
    navigate_(volume_);
    delete_menu_.Previous();
    break;
  default:
    break;
  }
}

auto Controller::Impl::AllDownEvent() -> void
{
  switch(state_)
  {
  case State::Start:
    navigate_(volume_);
    start_menu_.Next();
    break;
  case State::Pause:
    navigate_(volume_);
    pause_menu_.Next();
    break;
  case State::Chapter:
    navigate_(volume_);
    chapter_menu_.Next();
    break;
  case State::Load:
    navigate_(volume_);
    load_menu_.Next();
    break;
  case State::Delete:
    navigate_(volume_);
    delete_menu_.Next();
    break;
  default:
    break;
  }
}

auto Controller::Impl::AllSelectEvent() -> void
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
  case State::Delete:
    select_(volume_);
    delete_menu_.Select();
    break;
  case State::Story:
    back_(volume_);
    state_ = State::Pause;
    story_script_.Pause();
    pause_menu_[0];
    pause_script_.Resume();
    break;
  default:
    break;
  }
}

auto Controller::Impl::AllBackEvent() -> void
{
  switch(state_)
  {
  case State::Pause:
    back_(volume_);
    state_ = State::Story;
    story_script_.Resume();
    pause_script_.Pause();
    break;
  case State::PausePlayer:
    back_(volume_);
    state_ = State::Pause;
    break;
  case State::Player:
  case State::Load:
  case State::Chapter:
  case State::Delete:
    back_(volume_);
    state_ = State::Start;
    StartMenu();
    break;
  case State::Story:
    back_(volume_);
    state_ = State::Pause;
    story_script_.Pause();
    pause_menu_[0];
    pause_script_.Resume();
    break;
  default:
    break;
  }
}

auto Controller::Impl::AllChoiceSelectEvent() -> void
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
  case State::Delete:
    select_(volume_);
    delete_menu_.Select();
    break;
  default:
    break;  
  }
}

auto Controller::Impl::AllChoiceBackEvent() -> void
{
  switch(state_)
  {
  case State::Pause:
    back_(volume_);
    state_ = State::Story;
    story_script_.Resume();
    pause_script_.Pause();
    break;
  case State::PausePlayer:
    back_(volume_);
    state_ = State::Pause;
    break;
  case State::Player:
  case State::Load:
  case State::Chapter:
  case State::Delete:
    back_(volume_);
    state_ = State::Start;
    StartMenu();
    break;
  default:
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
  case State::Delete:
    window_.Clear();
    start_script_.Render();
    delete_menu_.Render();
    window_.Show();
    break;
  case State::Story:
    window_.Clear();
    story_script_.Render();
    window_.Show();
    break;
  case State::PausePlayer:
  case State::Player:
    window_.Clear();
    start_script_.Render();
    player_.Render();
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

auto Controller::Move(int id, float x, float y) -> void
{
  impl_->player_.Move(id, x, y);
}

auto Controller::Look(int id, float x, float y) -> void
{
  impl_->player_.Look(id, x, y);
}

auto Controller::ChoiceUp(int id) -> void
{
  impl_->player_.ChoiceUp(id);
}

auto Controller::ChoiceDown(int id) -> void
{
  impl_->player_.ChoiceDown(id);
}

auto Controller::ChoiceLeft(int id) -> void
{
  impl_->player_.ChoiceLeft(id);
}

auto Controller::ChoiceRight(int id) -> void
{
  impl_->player_.ChoiceRight(id);
}

auto Controller::ActionLeft(int id, bool state) -> void
{
  impl_->player_.ActionLeft(id, state);
}

auto Controller::ActionRight(int id, bool state) -> void
{
  impl_->player_.ActionRight(id, state);
}

auto Controller::Select(int id) -> void
{
  impl_->player_.Select(id);
}

auto Controller::Back(int id) -> void
{
  impl_->player_.Back(id);
}

auto Controller::Quit(event::Command const& command) -> void
{
  impl_->Quit(command);
}

auto Controller::Add(int id) -> void
{
  impl_->Add(id);
}

auto Controller::Remove(int id) -> void
{
  impl_->Remove(id);
}

Controller::operator bool() const
{
  return static_cast<bool>(impl_);
}
}