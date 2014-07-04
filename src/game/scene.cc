#include "scene.h"
#include <map>
#include "json_iterator.h"
namespace game
{
typedef std::multimap<int, event::Command> LayerMap;

class SceneImpl
{
public:
  SceneImpl(json::JSON const& json, display::Window& window);
  void Add(event::Command const& layer, int z);
  void Render(void);
  void Modulation(float r, float g, float b);
  LayerMap layers_;
  display::Modulation modulation_;
};

void SceneImpl::Render(void)
{
  for(auto iter = layers_.begin(); iter != layers_.end();)
  {
    if(iter->second())
    {
      ++iter;
    }
    else
    {
      iter = layers_.erase(iter);
    }
  }
}

void SceneImpl::Modulation(float r, float g, float b)
{
  modulation_.r(r);
  modulation_.g(g);
  modulation_.b(b);
}

void SceneImpl::Add(event::Command const& layer, int z)
{
  layers_.emplace(z, layer);
}

SceneImpl::SceneImpl(json::JSON const& json, display::Window& window)
{
  json_t* layers;
  double r, g, b;
  json.Unpack("{sos[fff]}", 
    "layers", &layers,
    "modulation", &r, &g, &b);
  modulation_ = display::Modulation(float(r), float(g), float(b), 1.f);

  for(json::JSON const& value : json::JSON(layers))
  {
    char const* image;
    int plane;
    double parallax;
    json_t* render_box;
    double angle;

    value.Unpack("{sssisfsosf}", 
      "image", &image,
      "z", &plane,
      "parallax", &parallax,
      "render box", &render_box,
      "angle", &angle);

    event::Command bind = std::bind(
      display::Texture(image, window),
      display::BoundingBox(),
      display::BoundingBox(json::JSON(render_box)),
      float(parallax),
      true,
      angle,
      modulation_);
    
    Add(bind, plane);
  }
}

void Scene::Render(void)
{
  impl_->Render();
}

void Scene::Add(event::Command const& layer, int z)
{
  impl_->Add(layer, z);
}

Scene::Scene(json::JSON const& json, display::Window& window)
{
  impl_ = std::make_shared<SceneImpl>(json, window);
}

void Scene::Modulation(float r, float g, float b)
{
  impl_->Modulation(r, g, b);
}
}