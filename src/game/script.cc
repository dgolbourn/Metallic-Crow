#include "script.h"
#include "music.h"
#include "hero.h"
#include "box.h"
#include "item.h"
#include "bind.h"
#include <vector>
namespace game
{
class ScriptImpl final : public std::enable_shared_from_this<ScriptImpl>
{
public:
  ScriptImpl(json::JSON const& json, Subtitle& subtitle, display::Window& window, Scene& scene, event::Queue& queue, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world, event::Event& event);
  void Init(dynamics::World& world);
  void Pause(void);
  void Resume(void);
  void View(void);
  bool paused_;
  audio::Music music_;
  std::vector<Item> items_;
  std::vector<Box> boxes_;
  Hero hero_;
  display::Window window_;
};

ScriptImpl::ScriptImpl(json::JSON const& json, Subtitle& subtitle, display::Window& window, Scene& scene, event::Queue& queue, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world, event::Event& event) : paused_(true), window_(window)
{
  audio::Music music("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/BassRockinDJJin-LeeRemix.mp3");
  music.Volume(0.5);
  music.Play();
  music.Pause();
  music_ = music;

  Hero hero(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/hero.json"), window, scene, dcollision, ccollision, queue, world, event);
  hero_ = hero;

  for(int i = 0; i < 100; ++i)
  {
    Box platform(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/platform.json"), window, scene, queue, dcollision, world);
    platform.Position(game::Position(float(200 + rand() % 10000), float(200 + rand() % 3000)));
    boxes_.push_back(platform);
  }

  for(int i = 0; i < 30; ++i)
  {
    Box box(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/box.json"), window, scene, queue, dcollision, world);
    box.Position(game::Position(float(200 + rand() % 10000), -400.f));
    boxes_.push_back(box);
  }

  Item item(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/watering_can.json"), window, scene, queue, ccollision, world);
  item.Position(game::Position(300.f, 300.f));
  items_.push_back(item);

  item.Hysteresis([&]()
  {
    subtitle.Text("Cat: This is a test!");
    subtitle.Choice("", "the quick brown fox...", "", "Such game!");
    subtitle.Down([&]()
    {
      subtitle.Text("Cat: The quick brown fox jumped over the lazy dog.");
      subtitle.Choice("", "", "", "done");
      subtitle.Right([&]()
      {
        subtitle.Text("Cat: And we're finished?");
        subtitle.Choice("yes!", "", "", "");
        items_.clear();
        return false;
      });
      return false;
    });
    return true;
  }, [&]()
  {
    subtitle.Clear();
    subtitle.Text("");
    subtitle.Choice("", "", "", "");
    return true;
  });
}

void ScriptImpl::View(void)
{
  game::Position p = hero_.Position();
  window_.View(p.first, p.second, 1.f);
}

void ScriptImpl::Pause(void)
{
  if(!paused_)
  {
    paused_ = true;
    music_.Pause();
    for(auto& item : items_)
    {
      item.Pause();
    }
    for(auto& box : boxes_)
    {
      box.Pause();
    }
    hero_.Pause();
  }
}

void ScriptImpl::Resume(void)
{
  if(paused_)
  {
    paused_ = false;
    music_.Resume();
    for(auto& item : items_)
    {
      item.Resume();
    }
    for(auto& box : boxes_)
    {
      box.Resume();
    }
    hero_.Resume();
  }
}

void ScriptImpl::Init(dynamics::World& world)
{
  auto ptr = shared_from_this();
  world.Add(event::Bind(&ScriptImpl::View, ptr));
}

void Script::Pause(void)
{
  impl_->Pause();
}

void Script::Resume(void)
{
  impl_->Resume();
}

Script::Script(json::JSON const& json, Subtitle& subtitle, display::Window& window, Scene& scene, event::Queue& queue, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world, event::Event& event)
{
  impl_ = std::make_shared<ScriptImpl>(json, subtitle, window, scene, queue, dcollision, ccollision, world, event);
  impl_->Init(world);
}
}