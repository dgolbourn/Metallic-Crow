#include "scene.h"
#include <map>
namespace game
{
namespace
{ 
typedef std::multimap<int, event::Command> Layers;
}

class Scene::Impl
{
public:
  void Add(event::Command const& layer, int z);
  void Render();
  Layers layers_;
};

void Scene::Impl::Render()
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

void Scene::Impl::Add(event::Command const& layer, int z)
{
  layers_.emplace(z, layer);
}

void Scene::Render()
{
  impl_->Render();
}

void Scene::Add(event::Command const& layer, int z)
{
  impl_->Add(layer, z);
}

Scene::Scene() : impl_(std::make_shared<Impl>())
{
}
}