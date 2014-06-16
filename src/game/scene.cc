#include "scene.h"
#include <map>
#include "jansson.h"

namespace game
{
typedef std::multimap<int, event::Command> LayerMap;

class SceneImpl
{
public:
  SceneImpl(json::JSON const& json, display::Window& window);
  void Add(event::Command const& layer, int z);
  void Render(void);
  LayerMap layers_;
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

void SceneImpl::Add(event::Command const& layer, int z)
{
  layers_.emplace(z, layer);
}

SceneImpl::SceneImpl(json::JSON const& json, display::Window& window)
{
  json_t* layers;
  json.Unpack("{so}", "layers", &layers);
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
      angle);
    
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
}