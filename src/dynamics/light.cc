#include "light.h"
#include <queue>
#include <map>
#include "world_impl.h"
#include "body_impl.h"
#include "ray_search.h"
#include "box_search.h"
#include "body_impl_iterator.h"

static void operator*=(b2Vec3& a, b2Vec3 const& b)
{
  a.x *= b.x;
  a.y *= b.y;
  a.z *= b.z;
}

namespace dynamics
{
typedef std::pair<BodyImpl*, BodyImpl*> BodyPair;
typedef std::map<BodyPair, b2Vec3> AttenuationMap;

struct LightPoint
{
  b2Vec3 emission;
  int hops;
};

typedef std::pair<BodyImpl*, LightPoint> BodyLight;
typedef std::queue<BodyLight> LightQueue;

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

static BodyPair Make(BodyImpl* body_a, BodyImpl* body_b)
{
  BodyPair ret;
  if(body_a < body_b)
  {
    ret = BodyPair(body_a, body_b);
  }
  else
  {
    ret = BodyPair(body_b, body_a);
  }
  return ret;
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
  BodyPair pair = Make(&source, &target);
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

//#pragma optimize( "", off )
void WorldImpl::Light(void)
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
      temp.emission = body.light_.emission;
      temp.hops = max_hops;
      sources.emplace(&body, temp);
    }
  }

  while(!sources.empty())
  {
    BodyLight source = sources.front();
    sources.pop();
 
    for(BodyImpl* target : BoxSearch(LightBox(source.second.emission, *source.first), world_))
    {
      if(source.first != target)
      {
        bool diffuse = (target->light_.diffuse) && (source.second.hops > 0);
        if(target->light_.illuminate || diffuse)
        {
          b2Vec3 incoming = Attenuation(*source.first, *target, attenuations, world_);
          incoming *= source.second.emission;

          if(target->light_.illuminate)
          {
            b2Vec3 illumination = incoming;
            illumination *= target->light_.absorption;
            target->light_.illumination += illumination;
          }

          if(diffuse)
          {
            LightPoint temp;
            temp.emission = incoming;
            temp.emission *= target->light_.diffusion;
            temp.hops = source.second.hops - 1;
            sources.emplace(target, temp);
          }
        }
      }
    }
  }
}

Light::Light(json::JSON const& json) : emit(false), transmit(false), diffuse(false), illuminate(false), illumination({0.f, 0.f, 0.f})
{
  json_t* emit_json;
  json_t* transmit_json;
  json_t* diffuse_json;
  json_t* intrinsic_json;
  json_t* absorb_json;
  json.Unpack("{sososososo}",
    "emit", &emit_json,
    "transmit", &transmit_json,
    "diffuse", &diffuse_json,
    "absorb", &absorb_json,
    "intrinsic", &intrinsic_json);

  if(json::JSON temp = json::JSON(emit_json))
  {
    emit = true;
    double x, y, z;
    temp.Unpack("[fff]", &x, &y, &z);
    emission.Set(float32(x), float32(y), float32(z));
  }

  if(json::JSON temp = json::JSON(transmit_json))
  {
    transmit = true;
    double x, y, z;
    temp.Unpack("[fff]", &x, &y, &z);
    transmission.Set(float32(x), float32(y), float32(z));
  }

  if(json::JSON temp = json::JSON(diffuse_json))
  {
    diffuse = true;
    double x, y, z;
    temp.Unpack("[fff]", &x, &y, &z);
    diffusion.Set(float32(x), float32(y), float32(z));
  }

  json::JSON temp_intrinsic = json::JSON(intrinsic_json);
  json::JSON temp_absorb = json::JSON(absorb_json);
  if(temp_intrinsic && temp_absorb)
  {
    illuminate = true;
    double x, y, z;
    temp_intrinsic.Unpack("[fff]", &x, &y, &z);
    intrinsic.Set(float32(x), float32(y), float32(z));

    temp_absorb.Unpack("[fff]", &x, &y, &z);
    absorption.Set(float32(x), float32(y), float32(z));
  }
}

Light::Light(void) : emit(false), transmit(false), diffuse(false), illuminate(false)
{
}
}