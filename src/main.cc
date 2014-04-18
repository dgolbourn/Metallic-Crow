#include <stdio.h>
#include <exception>
#include <iostream>
#include "window.h"
#include "signal.h"
#include "event.h"
#include "decoder.h"
#include "audio_format.h"
#include "music.h"
#include "sound.h"
#include "timer.h"
#include "animation.h"
#include "scene.h"
#include "command.h"
#include "hero.h"
#include "collision.h"
#include "enemy.h"
#include "bind.h"
#include "hud.h"
#include "rules_collision.h"
#include <chrono>
#include "dynamics_collision.h"
#include <thread>
#include "boost/exception/diagnostic_information.hpp"
#include "ffmpeg_exception.h"
#include "img_exception.h"
#include "world.h"
#include "box.h"
#include "item.h"
#include "command_collision.h"
#include "subtitle.h"
static bool run = true;
static bool Quit(void)
{
  run = false;
  return false;
}

int main(int argc, char* argv[]) 
{
  int ret;
  try
  {
    event::Event eL;
    event::Default();
    event::quit.Add(Quit);
    display::Window w(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/window.json"));
    display::Texture S("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/black.png", w);
    //audio::Music mixer("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/BassRockinDJJin-LeeRemix.mp3");
    audio::Music mixer("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/Boogie_Belgique_-_01_-_Forever_and_Ever.mp3");
    mixer.Volume(0.5);
    mixer.Play();
    mixer.Pause();
    event::pause.first.Add(event::Bind(&audio::Music::Pause, mixer));
    event::pause.second.Add(event::Bind(&audio::Music::Resume, mixer));
    game::Scene Sc(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/scene.json"), w);
    event::Queue queue;
    game::Collision col(queue);
    game::RulesCollision rc(col);
    game::DynamicsCollision dc(col);
    game::CommandCollision cc(col);
    dc.Link(0, 1);
    dc.Link(1, 1);
    dc.Link(0, 2);
    cc.Link(0, 3);
    cc.Link(0, 4);
    dynamics::World world(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/world.json"), col);
    game::Hero h(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/hero.json"), w, Sc, rc, dc, cc, queue, world);
    h.End(Quit);
    game::HUD hud(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/hud.json"), w, Sc);
    game::Subtitle sub(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/subtitle.json"), w, Sc, queue);
    h.Life(event::Bind(&game::HUD::Life, hud));
    
    std::vector<game::Box> platforms(100);
    for(auto& platform : platforms)
    {
      platform = game::Box(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/platform.json"), w, Sc, queue, dc, world);
      platform.Position(game::Position(float(200 + rand() % 10000), float(200 + rand() % 3000)));
    }

    std::vector<game::Box> boxes(30);
    for(auto& box : boxes)
    {
      box = game::Box(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/box.json"), w, Sc, queue, dc, world);
      box.Position(game::Position(float(200 + rand() % 10000), -400.f));
    }

    std::vector<game::Item> items(1);
    for(auto& item : items)
    {
      item = game::Item(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/watering_can.json"), w, Sc, queue, dc, cc, world);
      item.Position(game::Position(300.f, 300.f));
    }

    event::pause.second();

    sub.Choice("This is a test", "the quick brown fox...", "Jumps over the lazy dog...", "Such game!");
    //sub.Text("Cat: This is a test!\na b c d e f g h j i g f");

    std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point tick = last;
    int frame_rate = 60;
    std::chrono::milliseconds frame_period_ms(1000/frame_rate);
    float frame_period_s = 1.f/frame_rate;
    while(run)
    {
      std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
      float dt = std::chrono::duration_cast<std::chrono::duration<float>>(now - last).count();
      last = now;
      int current_frame_rate = int(std::round(1./dt));
      hud.Score(current_frame_rate);
      game::Position p = h.Position();
      w.View(p.first, p.second, 1.f);
      w.Clear();
      Sc.Render();
      w.Show();
      event::Check();
      queue();
      world.Step();
      tick += frame_period_ms;
      std::this_thread::sleep_until(tick);
    }
    ret = EXIT_SUCCESS;
  }
  catch(...)
  {
    std::cerr << "Unhandled exception:" << std::endl << boost::current_exception_diagnostic_information();
    ret = EXIT_FAILURE;
  }
  return ret;
}