#include "scene.h"
#include <map>
namespace
{ 
typedef std::multimap<int, event::Command> Layers;
}

namespace game
{
class Scene::Impl
{
public:
  auto Add(event::Command const& layer, int z) -> void;
  auto Render() -> void;
  Layers layers_;
};

auto Scene::Impl::Render() -> void
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

auto Scene::Impl::Add(event::Command const& layer, int z) -> void
{
  layers_.emplace(z, layer);
}

auto Scene::Render() -> void
{
  impl_->Render();
}

auto Scene::Add(event::Command const& layer, int z) -> void
{
  impl_->Add(layer, z);
}

Scene::Scene() : impl_(std::make_shared<Impl>())
{
}
}