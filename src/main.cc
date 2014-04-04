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
static bool run = true;
static bool Quit(void)
{
  run = false;
  return false;
}

int main(int argc, char *argv[]) 
{
  int ret;
  try
  {
    event::Event eL;
    event::Default();
    event::quit.Add(Quit);
    display::Window w(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/window.json"));
    display::Texture S = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/black.png");
    //audio::Music mixer("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/BassRockinDJJin-LeeRemix.mp3");
    audio::Music mixer("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/Boogie_Belgique_-_01_-_Forever_and_Ever.mp3");
    mixer.Volume(0.5);
    mixer.Play();
    mixer.Pause();
    event::pause.first.Add(event::Bind(&audio::Music::Pause, mixer));
    event::pause.second.Add(event::Bind(&audio::Music::Resume, mixer));
    game::Scene Sc(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/scene.json"), w);
    game::Collision col;
    game::RulesCollision rc(col);
    game::DynamicsCollision dc(col);
    dc.Link(0, 1);
    dc.Link(1, 2);
    dc.Link(2, 0);
    dc.Link(2, 2);
    event::Queue queue;
    dynamics::World world(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/world.json"), col);
    game::Hero h(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/hero.json"), w, Sc, rc, dc, queue, world);
    h.End(Quit);
    game::HUD hud(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/hud.json"), w, Sc);
    h.Life(event::Bind(&game::HUD::Life, hud));
    
    std::vector<display::BoundingBox> boxes(100);
    std::vector<dynamics::Body> bodies(100);
    int idx = 0;
    for(auto& box : boxes)
    {
      box = display::BoundingBox(200 + rand() % 1000, 200+rand() % 1000, 50, 50);
      event::Command c = std::bind(S, display::BoundingBox(), box, 1.f, false, 0.);
      Sc.Add(c, -1);
      if (idx & 5)
      {
        bodies[idx] = dynamics::Body((box.x() + 0.5f * box.w()) / 100.f, (box.y() + 0.5f* box.h()) / 100.f, 0.f, 0.f, box.w() / 100.f, box.h() / 100.f, std::numeric_limits<float>::infinity(), 0.1, 0.f, 0.f, world);
        dc.Add(1, bodies[idx]);
      }
      else
      {
        box.y(-box.y());
        bodies[idx] = dynamics::Body((box.x() + 0.5f * box.w()) / 100.f, (box.y() + 0.5f* box.h()) / 100.f, 0.f, 0.f, box.w() / 100.f, box.h() / 100.f, 1.0f, 0.1, 0.f, 0.f, world);
        dc.Add(2, bodies[idx]);
      }
      display::BoundingBox::WeakPtr b = box;
      dynamics::Body::WeakPtr d = bodies[idx];
      event::Command cc = [=](void)
      {
        game::Position p = d.Lock().Position();  
        display::BoundingBox c = b.Lock();
        c.x(p.first * 100.f - 0.5*c.w());
        c.y(p.second * 100.f - 0.5*c.h());
        return true; 
      };
      world.Add(cc);
      idx++;
    }

    event::pause.second();

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