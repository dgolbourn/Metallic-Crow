#include <stdio.h>
#include <exception>
#include <iostream>
#include "window.h"
#include "bounding_box.h"
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
#include "dynamics.h"
#include "dynamics_collision.h"

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
    display::Texture S = w.Load("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/petrified_rock_large__x1_iconic_png_1354840406.png");
    //audio::Music mixer("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/BassRockinDJJin-LeeRemix.mp3");
    audio::Music mixer("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/Boogie_Belgique_-_01_-_Forever_and_Ever.mp3");
    mixer.Volume(0.5);
    event::pause.first.Add(event::Bind(&audio::Music::Pause, mixer));
    event::pause.second.Add(event::Bind(&audio::Music::Resume, mixer));
    game::Scene Sc(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/scene.json"), w);
    game::Collision col;
    game::RulesCollision rc(col);
    rc.Link(0, 1);
    game::DynamicsCollision dc(col);
    dc.Link(0, 1);
    event::Queue queue;
    game::Hero h(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/hero.json"), w, Sc, rc, dc, queue);
    h.End(Quit);
    game::HUD hud(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/hud.json"), w, Sc);
    h.Life(event::Bind(&game::HUD::Life, hud));

    std::vector<display::BoundingBox> boxes(5000);
    std::vector<game::Dynamics> dynamics(5000);
    int idx = 0;
    for(auto& box : boxes)
    {
      box = display::BoundingBox(-1500 + (rand() & 3000), -1500 + (rand() % 3000), 50, 50);
      event::Command c = std::bind(S, display::BoundingBox(), box, 1.f, false, 0.);
      Sc.Add(c, -1);

      game::RulesCollision::Send send = [=](){return std::pair<game::RulesCollision::Rules, bool>(game::RulesCollision::Rules(rand() %10, 0), true); };
      game::RulesCollision::Receive receive = [=](game::RulesCollision::Rules const& rules){(void)rules;  std::cout << "hit!" << std::endl; return true; };
      game::RulesCollision::Channel channel(send, receive);
      rc.Add(1, box, channel);

      dynamics[idx] = game::Dynamics(box.x() + box.w() *.5f, box.y() + box.h() *0.5f, 0, 0, box.w()*.5f, box.h()*.5f, 0.5f);
      dc.Add(1, dynamics[idx], box);
      ++idx;
    }

    event::pause.second();
    int score = 0;
    std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
    float dt = 0.f;
    while(run)
    {
      hud.Score(int(1./dt));
      game::Position p = h.Position();
      w.View(p.first, p.second, 1.f);
      w.Clear();
      Sc.Render();
      w.Show();
      event::Check();
      queue();

      std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
      dt = std::chrono::duration_cast<std::chrono::duration<float>>(t1 - t0).count();
      t0 = t1;
      h.Step(dt);
      col.Check();
    }
    ret = EXIT_SUCCESS;
  }
  catch(std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    ret = EXIT_FAILURE;
  }
  catch(...)
  {
    ret = EXIT_FAILURE;
  }
  return ret;
}