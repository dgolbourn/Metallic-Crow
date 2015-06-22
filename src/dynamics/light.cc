#include "light.h"
#include <queue>
#include <map>
#include "world_impl.h"
#include "body_impl.h"
#include "ray_search.h"
#include "box_search.h"
#include "body_impl_iterator.h"
#include "body_impl_pair.h"

static void operator*=(b2Vec3& a, b2Vec3 const& b)
{
  a.x *= b.x;
  a.y *= b.y;
  a.z *= b.z;
}

namespace dynamics
{
typedef std::map<BodyImplPair, b2Vec3> AttenuationMap;

struct LightPoint
{
  BodyImpl* source;
  b2Vec3 emission;
  int hops;
};

typedef std::queue<LightPoint> LightQueue;

static const float32 min_illumination = .01f;

static b2AABB LightBox(b2Vec3 const& emission, BodyImpl const& body)
{
  float32 max_emission = std::max({emission.x, emission.y, emission.z});
  float32 max_distance = b2Sqrt(max_emission / min_illumination);
  b2Vec2 delta(max_distance, max_distance);
  b2Vec2 const& position = body.body_->GetPosition();
  b2AABB box;
  box.upperBound = position;
  box.upperBound += delta;
  box.lowerBound = position;
  box.lowerBound -= delta;
  return box;
}

static float32 Distance2(b2Vec2 const& body_a, b2Vec2 const& body_b)
{
  b2Vec2 distance = body_a;
  distance -= body_b;
  return distance.LengthSquared();
}

static b2Vec3 Attenuation(b2Vec2 const& body_a, b2Vec2 const& body_b)
{
  float32 temp = 1.f / (1.f + Distance2(body_a, body_b));
  return b2Vec3(temp, temp, temp);
}

static b2Vec3 Attenuation(BodyImpl& source, BodyImpl& target, AttenuationMap& map, b2World const& world)
{
  b2Vec3 attenuation;
  BodyImplPair pair = Make(&source, &target);
  auto iter = map.find(pair);
  if(iter == map.end())
  {
    b2Vec2 const& source_pos = source.body_->GetPosition();
    b2Vec2 const& target_pos = target.body_->GetPosition();
    attenuation = Attenuation(source_pos, target_pos);
    for(BodyImpl* obstacle : RaySearch(source_pos, target_pos, world))
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

static const int max_hops = 3;

void WorldImpl::Light()
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

      float32 x;
      {
        lua::Guard guard = lua.Field(1);
        lua.Pop(x);
      }

      float32 y;
      {
        lua::Guard guard = lua.Field(2);
        lua.Pop(y);
      }

      float32 z;
      {
        lua::Guard guard = lua.Field(3);
        lua.Pop(z);
      }

      emission.Set(x, y, z);
    }
  }

  {
    lua::Guard guard = lua.Field("transmit");
    if(lua.Check())
    {
      transmit = true;

      float32 x;
      {
        lua::Guard guard = lua.Field(1);
        lua.Pop(x);
      }

      float32 y;
      {
        lua::Guard guard = lua.Field(2);
        lua.Pop(y);
      }

      float32 z;
      {
        lua::Guard guard = lua.Field(3);
        lua.Pop(z);
      }
      transmission.Set(x, y, z);
    }
  }

  {
    lua::Guard guard = lua.Field("diffuse");
    if(lua.Check())
    {
      diffuse = true;

      float32 x;
      {
        lua::Guard guard = lua.Field(1);
        lua.Pop(x);
      }

      float32 y;
      {
        lua::Guard guard = lua.Field(2);
        lua.Pop(y);
      }

      float32 z;
      {
        lua::Guard guard = lua.Field(3);
        lua.Pop(z);
      }

      diffusion.Set(x, y, z);
    }
  }

  {
    lua::Guard guard = lua.Field("intrinsic");
    if(illuminate = lua.Check())
    {
      float32 x;
      {
        lua::Guard guard = lua.Field(1);
        lua.Pop(x);
      }

      float32 y;
      {
        lua::Guard guard = lua.Field(2);
        lua.Pop(y);
      }

      float32 z;
      {
        lua::Guard guard = lua.Field(3);
        lua.Pop(z);
      }

      intrinsic.Set(x, y, z);
    }
  }

  if(illuminate)
  {
    lua::Guard guard = lua.Field("absorb");
    if(illuminate = lua.Check())
    {
      float32 x;
      {
        lua::Guard guard = lua.Field(1);
        lua.Pop(x);
      }

      float32 y;
      {
        lua::Guard guard = lua.Field(2);
        lua.Pop(y);
      }

      float32 z;
      {
        lua::Guard guard = lua.Field(3);
        lua.Pop(z);
      }

      absorption.Set(x, y, z);
    }
  }
}


Light::Light() : emit(false), transmit(false), diffuse(false), illuminate(false)
{
}
}