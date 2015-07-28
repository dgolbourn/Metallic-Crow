#include "scene.h"
#include "boost/bimap/bimap.hpp"
#include "boost/bimap/multiset_of.hpp" 
#include "boost/bimap/unordered_set_of.hpp" 
namespace
{ 
typedef boost::bimaps::bimap<boost::bimaps::multiset_of<float>, boost::bimaps::unordered_set_of<game::Actor>> Layers;
}

namespace game
{
class Scene::Impl
{
public:
  auto Add(Actor const& actor, float plane) -> void;
  auto Update(Actor const& actor, float plane) -> void;
  auto Remove(Actor const& actor) -> void;
  auto Render() -> void;
  Layers layers_;
};

auto Scene::Impl::Render() -> void
{
  for(auto actor : layers_.left)
  {
    actor.second.Render();
  }
}

auto Scene::Impl::Add(Actor const& actor, float plane) -> void
{
  layers_.left.insert(Layers::left_value_type(plane, actor));
}

auto Scene::Impl::Update(Actor const& actor, float plane) -> void
{
  layers_.right.replace_data(layers_.right.find(actor), plane);
}

auto Scene::Impl::Remove(Actor const& actor) -> void
{
  layers_.right.erase(actor);
}

auto Scene::Render() -> void
{
  impl_->Render();
}

auto Scene::Add(Actor const& actor, float plane) -> void
{
  impl_->Add(actor, plane);
}

auto Scene::Update(Actor const& actor, float plane) -> void
{
  impl_->Update(actor, plane);
}

auto Scene::Remove(Actor const& actor) -> void
{
  impl_->Remove(actor);
}

Scene::Scene() : impl_(std::make_shared<Impl>())
{
}
}