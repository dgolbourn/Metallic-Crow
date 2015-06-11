#ifndef LIGHT_H_
#define LIGHT_H_
#include "Box2D/Box2D.h"
#include "lua_stack.h"
namespace dynamics
{
struct Light
{
  Light();
  Light(lua::Stack& lua);

  b2Vec3 illumination;

  bool emit;
  b2Vec3 emission;

  bool transmit;
  b2Vec3 transmission;

  bool diffuse;
  b2Vec3 diffusion;

  bool illuminate;
  b2Vec3 intrinsic;
  b2Vec3 absorption;
};
}
#endif