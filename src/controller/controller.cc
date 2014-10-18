#include "controller.h"
#include "bind.h"
#include "window.h"
#include "menu.h"
#include "script.h"
#include "signal.h"
#include <vector>
#include "saves.h"
#include "json_iterator.h"
#include <sstream>
#include "time.h"
namespace game
{
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

void Chapters(json::JSON& json, Strings& names, Strings& files)
{
  for(json::JSON const& element : json)
  {
    char const* name;
    char const* file;
    element.Unpack("{ssss}", 
      "name", &name,
      "file", &file);
    names.emplace_back(name);
    files.emplace_back(file);
  }
}

void ChapterOptions(Menu& menu, int progress, Strings const& chapters)
{
  Strings options;
  for(int i = 0; i <= progress; ++i)
  {
    options.push_back(chapters[i]);
  }
  menu(options);
}

void LoadOptions(Menu& menu, Saves& saves, Strings const& chapters)
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
      
      if(int(chapters.size()) == progress)
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

class Controller::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(json::JSON const& json, event::Queue& queue);
  void Init();
  void Up();
  void Down();
  void Left();
  void Right();
  void ChoiceUp();
  void ChoiceDown();
  void ChoiceLeft();
  void ChoiceRight();
  void Select();
  void Render();
  void Add(event::Command const& command);

  void PauseContinue();
  void PauseMainMenu();
  void StartPlay();
  void StartChooseChapter();
  void StartLoad();
  void StartQuit();
  void Load(int slot);
  void Chapter(int chapter);
  void ChapterEnd();

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
  Strings chapter_files_;
  int slot_;
};

void Controller::Impl::Load(int slot)
{
  saves_.Stop();
  slot_ = slot;
  LoadOptions(chapter_menu_, saves_, chapter_names_);
  state_ = State::Start;
}

void Controller::Impl::Chapter(int chapter)
{
  saves_.Stop();
  saves_.Current(slot_, chapter);
  ChapterOptions(chapter_menu_, saves_.Progress(slot_), chapter_names_);
  state_ = State::Start;
}

Controller::Impl::Impl(json::JSON const& json, event::Queue& queue) : state_(State::Start), queue_(queue)
{
  json_t* window;
  json_t* menu;
  char const* pause;
  char const* start;
  char const* saves;
  json_t* chapters;
  json.Unpack("{sosossssssso}",
    "window", &window,
    "menu", &menu,
    "pause menu script", &pause,
    "start menu script", &start,
    "saves", &saves,
    "chapters", &chapters);

  window_ = display::Window(json::JSON(window));

  pause_menu_ = Menu(json::JSON(menu), window_);
  pause_menu_({"Continue", "Main Menu"});
  pause_script_ = Script(pause, window_, queue_);

  start_menu_ = Menu(json::JSON(menu), window_);
  start_menu_({"Play", "Chapters", "Load", "Quit"});
  start_script_ = Script(start, window_, queue_);

  saves_ = Saves(saves);
  slot_ = saves_.LastPlayed();

  Chapters(json::JSON(chapters), chapter_names_, chapter_files_);

  chapter_menu_ = Menu(json::JSON(menu), window_);
  ChapterOptions(chapter_menu_, saves_.Progress(slot_), chapter_names_);

  load_menu_ = Menu(json::JSON(menu), window_);
  LoadOptions(load_menu_, saves_, chapter_names_);
}

void Controller::Impl::Init()
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

void Controller::Impl::ChapterEnd()
{
  saves_.Current(slot_, saves_.Current(slot_) + 1);
  saves_.Save();
  if(saves_.Current(slot_) >= int(chapter_files_.size()))
  {
    saves_.Stop();
    story_script_ = Script();
    state_ = State::Start;
  }
  else
  {
    story_script_ = Script(chapter_files_[saves_.Current(slot_)], window_, queue_);
    story_script_.Add(function::Bind(&Impl::ChapterEnd, shared_from_this()));
    story_script_.Resume();
  }
}

void Controller::Impl::PauseContinue()
{
  state_ = State::Story;
  pause_script_.Pause();
  story_script_.Resume();
}

void Controller::Impl::PauseMainMenu()
{
  saves_.Stop();
  state_ = State::Start;
  pause_script_.Pause();
  start_script_.Resume();
}

void Controller::Impl::StartPlay()
{
  state_ = State::Story;
  start_script_.Pause();
  if(saves_.Current(slot_) >= int(chapter_files_.size()))
  {
    saves_.Current(slot_, 0);
  }
  saves_.Play(slot_);
  saves_.Save();
  story_script_ = Script(chapter_files_[saves_.Current(slot_)], window_, queue_);
  story_script_.Add(function::Bind(&Impl::ChapterEnd, shared_from_this()));
  story_script_.Resume();
}

void Controller::Impl::StartChooseChapter()
{
  state_ = State::Chapter;
}

void Controller::Impl::StartLoad()
{
  state_ = State::Load;
}

void Controller::Impl::StartQuit()
{
  signal_();
}

void Controller::Impl::Add(event::Command const& command)
{
  signal_.Add(command);
}

void Controller::Impl::Up()
{
  switch(state_)
  {
  case State::Start:
    start_menu_.Previous();
    break;
  case State::Pause:
    pause_menu_.Previous();
    break;
  case State::Story:
    story_script_.Up();
    break;
  case State::Chapter:
    chapter_menu_.Previous();
    break;
  case State::Load:
    load_menu_.Previous();
    break;
  default:
    break;
  }
}

void Controller::Impl::Down()
{
  switch(state_)
  {
  case State::Start:
    start_menu_.Next();
    break;
  case State::Pause:
    pause_menu_.Next();
    break;
  case State::Story:
    story_script_.Down();
    break;
  case State::Chapter:
    chapter_menu_.Next();
    break;
  case State::Load:
    load_menu_.Next();
    break;
  default:
    break;
  }
}

void Controller::Impl::Left()
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
    story_script_.Left();
    break;
  }
}

void Controller::Impl::Right()
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
    story_script_.Right();
    break;
  }
}

void Controller::Impl::ChoiceUp()
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

void Controller::Impl::ChoiceDown()
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
    story_script_.ChoiceDown();
    break;
  }
}

void Controller::Impl::ChoiceLeft()
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

void Controller::Impl::ChoiceRight()
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
    story_script_.ChoiceRight();
    break;
  }
}

void Controller::Impl::Select()
{
  switch(state_)
  {
  case State::Start:
    start_menu_.Select();
    break;
  case State::Pause:
    pause_menu_.Select();
    break;
  case State::Story:
    state_ = State::Pause;
    story_script_.Pause();
    pause_script_.Resume();
    break;
  case State::Chapter:
    chapter_menu_.Select();
    break;
  case State::Load:
    load_menu_.Select();
    break;
  default:
    break;
  }
}

void Controller::Impl::Render()
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

Controller::Controller(json::JSON const& json, event::Queue& queue) : impl_(std::make_shared<Impl>(json, queue))
{
  impl_->Init();
}

void Controller::Up()
{
  impl_->Up();
}

void Controller::Down()
{
  impl_->Down();
}

void Controller::Left()
{
  impl_->Left();
}

void Controller::Right()
{
  impl_->Right();
}

void Controller::ChoiceUp()
{
  impl_->ChoiceUp();
}

void Controller::ChoiceDown()
{
  impl_->ChoiceDown();
}

void Controller::ChoiceLeft()
{
  impl_->ChoiceLeft();
}

void Controller::ChoiceRight()
{
  impl_->ChoiceRight();
}

void Controller::Select()
{
  impl_->Select();
}

void Controller::Add(event::Command const& command)
{
  impl_->Add(command);
}

Controller::operator bool() const
{
  return bool(impl_);
}
}