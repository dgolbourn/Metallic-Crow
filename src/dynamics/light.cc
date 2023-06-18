#include "light.h"
#include <queue>
#include <map>
#include "world_impl.h"
#include "body_impl.h"
#include "ray_search.h"
#include "box_search.h"
#include "body_impl_iterator.h"
#include "body_impl_pair.h"
namespace
{
auto operator*=(b2Vec3& a, b2Vec3 const& b) -> void
{
  a.x *= b.x;
  a.y *= b.y;
  a.z *= b.z;
}

typedef std::map<dynamics::BodyImplPair, b2Vec3> AttenuationMap;

struct LightPoint
{
  dynamics::BodyImpl* source;
  b2Vec3 emission;
  int hops;
};

typedef std::queue<LightPoint> LightQueue;

const float min_illumination = .001f;

auto LightBox(b2Vec3 const& emission, dynamics::BodyImpl const& body) -> b2AABB
{
  float max_emission = std::max({emission.x, emission.y, emission.z});
  float max_distance = b2Sqrt(max_emission / min_illumination);
  b2Vec2 delta(max_distance, max_distance);
  b2Vec2 const& position = body.body_->GetPosition();
  b2AABB box;
  box.upperBound = position;
  box.upperBound += delta;
  box.lowerBound = position;
  box.lowerBound -= delta;
  return box;
}

auto Distance2(b2Vec2 const& body_a, b2Vec2 const& body_b) -> float
{
  b2Vec2 distance = body_a;
  distance -= body_b;
  return distance.LengthSquared();
}

auto Attenuation(b2Vec2 const& body_a, b2Vec2 const& body_b) -> b2Vec3
{
  float temp = 1.f / (1.f + Distance2(body_a, body_b));
  return b2Vec3(temp, temp, temp);
}

auto Attenuation(dynamics::BodyImpl& source, dynamics::BodyImpl& target, AttenuationMap& map, b2World const& world) -> b2Vec3 
{
  b2Vec3 attenuation;
  dynamics::BodyImplPair pair = Make(&source, &target);
  auto iter = map.find(pair);
  if(iter == map.end())
  {
    b2Vec2 const& source_pos = source.body_->GetPosition();
    b2Vec2 const& target_pos = target.body_->GetPosition();
    attenuation = Attenuation(source_pos, target_pos);
    for(dynamics::BodyImpl* obstacle : dynamics::RaySearch(source_pos, target_pos, world))
    {
      if(obstacle->light_.transmit && (obstacle != &target))
      {
        attenuation *= obstacle->light_.transmission;
      }
    }
    map.emplace(pair, attenuation);
  }
  else
  {
    attenuation = iter->second;
  }
  return attenuation;
}

const int max_hops = 5;
}

namespace dynamics
{
auto WorldImpl::Light() -> void
{ 
  AttenuationMap attenuations;
  LightQueue sources;

  for(BodyImpl& body : world_)
  {
    if(body.light_.illuminate)
    {
      body.light_.illumination = body.light_.intrinsic;
      body.light_.illumination += ambient_;
    }

    if(body.light_.emit)
    {
      LightPoint temp;
      temp.source = &body;
      temp.emission = body.light_.emission;
      temp.hops = max_hops;
      sources.emplace(temp);
    }
  }

  while(!sources.empty())
  {
    LightPoint source = sources.front();
    sources.pop();
 
    for(BodyImpl* target : BoxSearch(LightBox(source.emission, *source.source), world_))
    {
      if(source.source != target)
      {
        bool diffuse = (target->light_.diffuse) && (source.hops > 0);
        if(target->light_.illuminate || diffuse)
        {
          b2Vec3 incoming = Attenuation(*source.source, *target, attenuations, world_);
          incoming *= source.emission;

          if(target->light_.illuminate)
          {
            b2Vec3 illumination = incoming;
            illumination *= target->light_.absorption;
            target->light_.illumination += illumination;
          }

          if(diffuse)
          {
            LightPoint temp;
            temp.source = target;
            temp.emission = incoming;
            temp.emission *= target->light_.diffusion;
            temp.hops = source.hops - 1;
            sources.emplace(temp);
          }
        }
      }
    }
  }
}

Light::Light(lua::Stack& lua) : emit(false), transmit(false), diffuse(false), illuminate(false), illumination({ 1.f, 1.f, 1.f })
{
  {
    lua::Guard guard = lua.Field("emit");
    if(lua.Check())
    {
      emit = true;
      emission.Set(lua.Field<float>(1), lua.Field<float>(2), lua.Field<float>(3));
    }
  }

  {
    lua::Guard guard = lua.Field("transmit");
    if(lua.Check())
    {
      transmit = true;
      transmission.Set(lua.Field<float>(1), lua.Field<float>(2), lua.Field<float>(3));
    }
  }

  {
    lua::Guard guard = lua.Field("diffuse");
    if(lua.Check())
    {
      diffuse = true;
      diffusion.Set(lua.Field<float>(1), lua.Field<float>(2), lua.Field<float>(3));
    }
  }

  {
    lua::Guard guard = lua.Field("intrinsic");
    illuminate = lua.Check();
    if(illuminate)
    {
      intrinsic.Set(lua.Field<float>(1), lua.Field<float>(2), lua.Field<float>(3));
    }
  }

  if(illuminate)
  {
    lua::Guard guard = lua.Field("absorb");
    illuminate = lua.Check();
    if(illuminate)
    {
      absorption.Set(lua.Field<float>(1), lua.Field<float>(2), lua.Field<float>(3));
    }
  }
}


Light::Light() : emit(false), transmit(false), diffuse(false), illuminate(false)
{
}
}