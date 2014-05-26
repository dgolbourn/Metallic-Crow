#include "script.h"
#include "music.h"
#include "hero.h"
#include "box.h"
#include "item.h"
#include "bind.h"
#include <vector>
#include "terrain.h"
#include "timer.h"
#include <boost/graph/adjacency_list.hpp>
#include <map>
#include "exception.h"
namespace game
{
class EdgeProperties
{
public:
  std::string type_;
  bool immediate_;

  EdgeProperties(std::string const& type, bool immediate) : type_(type), immediate_(immediate)
  {
  }

  EdgeProperties(json::JSON const& json)
  {
    char const* type;
    int immediate;
    json.Unpack("{sssb}",
      "name", &type,
      "immediate", &immediate);
    type_ = std::string(type);
    immediate_ = (immediate != 0);
  }
};

class VertexProperties
{
public:
  std::string text_;
  std::string up_;
  std::string down_;
  std::string left_;
  std::string right_;
  int interval_;
  float zoom_;
  Position focus_;
  bool subject_focus_;
  bool subject_hero_;

  VertexProperties(json::JSON const& json)
  {
    char const* text;
    char const* up;
    char const* down;
    char const* left;
    char const* right;
    double zoom;
    double focusx, focusy;
    char const* subject;
    json.Unpack("{sssssssssssisfs[ff]ss}",
      "text", &text,
      "up", &up,
      "down", &down,
      "left", &left,
      "right", &right,
      "timer", &interval_,
      "zoom", &zoom,
      "focus", &focusx, &focusy,
      "subject", &subject);
    text_ = std::string(text);
    up_ = std::string(up);
    down_ = std::string(down);
    left_ = std::string(left);
    right_ = std::string(right);
    zoom_ = float(zoom);
    focus_ = Position(float(focusx), float(focusy));
    std::string temp(subject);
    if(temp == "hero")
    {
      subject_hero_ = true;
      subject_focus_ = false;
    }
    else if(temp == "focus")
    {
      subject_hero_ = false;
      subject_focus_ = true;
    }
    else if(temp == "both")
    {
      subject_hero_ = true;
      subject_focus_ = true;
    }
    else
    {
      subject_hero_ = false;
      subject_focus_ = false;
    }
  }

  VertexProperties() :
    zoom_(1.f),
    focus_(0.f,0.f),
    subject_focus_(false),
    subject_hero_(false),
    text_(""),
    up_(""),
    down_(""),
    left_(""),
    right_(""),
    interval_(0)
  {
  }
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexProperties, EdgeProperties> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;

static void Add(Graph& graph, Vertex source, Vertex target, EdgeProperties const& edge)
{
  if(boost::add_edge(source, target, edge, graph).second == false)
  {
    BOOST_THROW_EXCEPTION(exception::Exception());
  }
}

class ScriptImpl final : public std::enable_shared_from_this<ScriptImpl>
{
public:
  ScriptImpl(display::Window& window, Subtitle& subtitle, event::Queue& queue);
  void Init(json::JSON const& json, Subtitle& subtitle, display::Window& window, Scene& scene, event::Queue& queue, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world, event::Event& event);
  void Pause(void);
  void Resume(void);
  void View(void);
  void Continue(Vertex start);
  void Event(std::string const& edge);
  void Update(VertexProperties const& vertex, bool immediate);
  bool paused_;
  audio::Music music_;
  std::vector<Item> items_;
  std::vector<Box> boxes_;
  std::vector<Terrain> terrain_;
  Hero hero_;
  display::Window window_;
  Subtitle subtitle_;
  event::Timer timer_;
  event::Queue queue_;
  Graph graph_;
  std::map<Vertex, Vertex> start_;
  Vertex current_;
  Position focus_;
  bool subject_focus_;
  bool subject_hero_;
  float zoom_;
};

ScriptImpl::ScriptImpl(display::Window& window, Subtitle& subtitle, event::Queue& queue) : paused_(true), window_(window), subtitle_(subtitle), queue_(queue), zoom_(1.f), focus_(0.f, 0.f), subject_hero_(false), subject_focus_(false), graph_()
{
}

void ScriptImpl::Init(json::JSON const& json, Subtitle& subtitle, display::Window& window, Scene& scene, event::Queue& queue, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world, event::Event& event)
{
  auto ptr = shared_from_this();
  world.Add(event::Bind(&ScriptImpl::View, ptr));
  subtitle_.Up(event::Bind(&ScriptImpl::Event, ptr, "up"));
  subtitle_.Down(event::Bind(&ScriptImpl::Event, ptr, "down"));
  subtitle_.Left(event::Bind(&ScriptImpl::Event, ptr, "left"));
  subtitle_.Right(event::Bind(&ScriptImpl::Event, ptr, "right"));

  audio::Music music("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/BassRockinDJJin-LeeRemix.mp3");
  music.Volume(0.01);
  music.Play();
  music.Pause();
  music_ = music;

  Hero hero(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/hero.json"), window, scene, dcollision, ccollision, queue, world, event);
  hero_ = hero;

  terrain_.push_back(Terrain(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/skyline.json"), window, scene, dcollision, world));
  terrain_.push_back(Terrain(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/boundary.json"), window, scene, dcollision, world));
  terrain_.push_back(Terrain(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/bushes.json"), window, scene, dcollision, world));
  terrain_.push_back(Terrain(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/grass.json"), window, scene, dcollision, world));
  terrain_.push_back(Terrain(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/tree1.json"), window, scene, dcollision, world));
  terrain_.push_back(Terrain(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/tree2.json"), window, scene, dcollision, world));

  {
    VertexProperties p1;
    p1.text_ = "yup!";
    p1.right_ = "hello!";
    Vertex v1 = boost::add_vertex(p1, graph_);
    start_[v1] = v1;

    VertexProperties p2;
    p2.text_ = "hello!";
    p2.up_ = "yup!";
    Vertex v2 = boost::add_vertex(p2, graph_);

    VertexProperties p3;
    Vertex v3 = boost::add_vertex(p3, graph_);

    EdgeProperties e12("right", true);
    Add(graph_, v1, v2, e12);
    EdgeProperties e23("up", true);
    Add(graph_, v2, v3, e23);
    EdgeProperties e11("exit", false);
    Add(graph_, v1, v1, e11);
    EdgeProperties e21("exit", false);
    Add(graph_, v2, v1, e21);
    EdgeProperties e33("exit", false);
    Add(graph_, v3, v3, e33);

    Item crow(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/crow.json"), window, scene, queue, ccollision, world);
    crow.Hysteresis(event::Bind(&ScriptImpl::Continue, ptr, v1), event::Bind(&ScriptImpl::Event, ptr, "exit"));
    items_.push_back(crow);
  }

  {
    Vertex vertex = boost::add_vertex(VertexProperties(), graph_);
    start_[vertex] = vertex;
    Item house(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/house.json"), window, scene, queue, ccollision, world);
    house.Hysteresis(event::Bind(&ScriptImpl::Continue, ptr, vertex), event::Bind(&ScriptImpl::Event, ptr, "exit"));
    items_.push_back(house);
  }

  {
    Vertex vertex = boost::add_vertex(VertexProperties(), graph_);
    start_[vertex] = vertex;
    Item rooster(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/rooster.json"), window, scene, queue, ccollision, world);
    rooster.Hysteresis(event::Bind(&ScriptImpl::Continue, ptr, vertex), event::Bind(&ScriptImpl::Event, ptr, "exit"));
    items_.push_back(rooster);
  }
}

void ScriptImpl::Continue(Vertex start)
{
  current_ = start;
  bool found = false;
  auto iter = start_.find(current_);
  if(iter != start_.end())
  {
    Update(graph_[iter->second], true);
    found = true;
  }
  if(!found)
  {
    BOOST_THROW_EXCEPTION(exception::Exception());
  }
}

void ScriptImpl::Event(std::string const& edge)
{
  bool found = false;
  auto keyvalue = start_.find(current_);
  if(keyvalue != start_.end())
  {
    for(auto iter = boost::out_edges(keyvalue->second, graph_); iter.first != iter.second; ++iter.first)
    {
      Edge out = *iter.first;
      if(graph_[out].type_ == edge)
      {
        Vertex next = boost::target(out, graph_);
        keyvalue->second = next;
        Update(graph_[next], graph_[out].immediate_);
        found = true;
        break;
      }
    }
  }
  if(!found)
  {
    BOOST_THROW_EXCEPTION(exception::Exception());
  }
}

void ScriptImpl::Update(VertexProperties const& vertex, bool immediate)
{
  if(immediate)
  {
    subtitle_.Text(vertex.text_);
    subtitle_.Choice(vertex.up_, vertex.down_, vertex.left_, vertex.right_);
    focus_ = vertex.focus_;
    subject_focus_ = vertex.subject_focus_;
    subject_hero_ = vertex.subject_hero_;
    zoom_ = vertex.zoom_;
    if(vertex.interval_ > 0)
    {
      timer_ = event::Timer(vertex.interval_, queue_);
      timer_.Add(event::Bind(&ScriptImpl::Event, shared_from_this(), "timer"));
      timer_.Play(0);
    }
    else
    {
      timer_ = event::Timer();
    }
  }
  else
  {
    subtitle_.Text("");
    subtitle_.Choice("", "", "", "");
    timer_ = event::Timer();
    subject_hero_ = false;
    subject_focus_ = false;
    zoom_ = 1.f;
  }
}

void ScriptImpl::View(void)
{
  game::Position p;
  if(subject_hero_)
  {
    p = hero_.Position();
    if(subject_focus_)
    {
      p.first += focus_.first;
      p.first *= .5f;
      p.second += focus_.second;
      p.second *= .5f;
    }
  }
  else if(subject_focus_)
  {
    p = focus_;
  }
  else
  {
    p = hero_.Position();
    p.second -= 200.f;
  }
  window_.View(p.first, p.second, zoom_);
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
    if(timer_)
    {
      timer_.Pause();
    }
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
    if(timer_)
    {
      timer_.Resume();
    }
  }
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
  impl_ = std::make_shared<ScriptImpl>(window, subtitle, queue);
  impl_->Init(json, subtitle, window, scene, queue, dcollision, ccollision, world, event);
}
}