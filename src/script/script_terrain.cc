#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::TerrainInit()
{
  lua_.Add(function::Bind(&Impl::TerrainLoad, shared_from_this()), "terrain_load", 0);
  lua_.Add(function::Bind(&Impl::TerrainFree, shared_from_this()), "terrain_free", 0);
  lua_.Add(function::Bind(&Impl::TerrainLight, shared_from_this()), "terrain_light", 0);
}

void Script::Impl::TerrainLoad()
{
  std::string name;
  std::string file;
  lua_.PopFront(name);
  lua_.PopFront(file);
  stage_->terrain_.emplace(name, Terrain(json::JSON(file), window_, stage_->scene_, stage_->group_, stage_->world_));
}

void Script::Impl::TerrainFree()
{
  std::string name;
  lua_.PopFront(name);
  stage_->terrain_.erase(name);
}

void Script::Impl::TerrainLight()
{
  std::string name;
  float r, g, b;
  lua_.PopFront(name);
  lua_.PopFront(r);
  lua_.PopFront(g);
  lua_.PopFront(b);
  auto range = stage_->terrain_.equal_range(name);
  for(auto& terrain = range.first; terrain != range.second; ++terrain)
  {
    terrain->second.Modulation(r, g, b);
  }
}
}