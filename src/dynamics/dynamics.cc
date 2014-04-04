#include "dynamics.h"
#include <cmath>
#include "world.h"
#include <iostream>

#include <queue>

namespace game
{
class DynamicsImpl
{
public:
  DynamicsImpl(float x, float y, float u, float v, float w, float h, float c, float m);
  void UpdatePosition(float t);
  void UpdateVelocity(float t);
  void Collision(DynamicsImpl& other);
  void Reset(void);
  float x_;
  float y_;
  float u_;
  float v_;
  float w_;
  float h_;
  float c_;
  float m_;
  float t_;
  float a_;
  float b_;
  float d_;
  float k_;
  float f_;
  float g_;
  //float x0_;
  //float y0_;
  float u0_;
  float v0_;
  float f0_;
  float g0_;

  bool hard_x;
  bool hard_y;

  dynamics::World b;
  dynamics::Body bb;
};

void DynamicsImpl::UpdateVelocity(float t)
{
  if(t < 0.f)
  {
    u_ = u0_;
    v_ = v0_;
    f_ = f0_;
    g_ = g0_;
  }
  else
  {
    float f = std::copysign(f_, u_) + d_ * u_;
    float g = std::copysign(g_, v_) + d_ * v_;
    u0_ = u_;
    v0_ = v_;
    u_ += (a_ - f / m_) * t;
    v_ += (b_ - g / m_) * t;
    f0_ = f_;
    g0_ = g_;
    f_ = 0.f;
    g_ = 0.f;
  }
}

void DynamicsImpl::UpdatePosition(float t)
{
  if(t < 0.f)
  {
    x_ += u0_ * t;// +0.5 * a_* t * t;
    y_ += v0_ * t;// +0.5 * b_* t * t;
  }
  else
  {
    //x0_ = x_;
    //y0_ = y_;
    x_ += u_ * t;
    y_ += v_ * t;
  }
}

void DynamicsImpl::Reset(void)
{
  //x_ = x0_;
  //y_ = y0_;
  u_ = u0_;
  v_ = v0_;
  f_ = f0_;
  g_ = g0_;
}

static float CollisionTime(float x0, float u0, float w0, float x1, float u1, float w1)
{
  float dx = x1 - x0;
  float du = u1 - u0;
  float dw = 0.5f * (w1 + w0);
  if(du < 0.f)
  {
    dx -= dw;
  }
  else
  {
    dx += dw;
  }
  return dx / du;
}

static float Collision(float u0, float m0, float c0, float u1, float m1, float c1)
{
  float u;
  if(std::isfinite(m0))
  {

    //c0 = 1.0f;
    //c1 = 1.0f;
    if(std::isfinite(m1))
    {
      u = (c0 * c1 * m1 * (u1 - u0) + m0 * u0 + m1 * u1) / (m0 + m1);
    }
    else
    {
      u = c0 * c1 * (u1 - u0) + u1;
    }
  }
  else
  {
    u = u0;
  }
  return u;
}

static float Friction(float a0, float m0, float k0, float a1, float m1, float k1)
{
  float f = 0.f;
  if(std::isfinite(m0))
  {
    f += m0 * a0;
  }
  if(std::isfinite(m1))
  {
    f -= m1 * a1;
  }
  return k0 * k1 * f;
}

static void Collision(float t, bool axis, DynamicsImpl& this_, DynamicsImpl& other)
{
  this_.UpdatePosition(-t);
  other.UpdatePosition(-t);
  this_.UpdateVelocity(-t);
  other.UpdateVelocity(-t);
  float f = 0.f;
  float g = 0.f;
  if(axis)
  {
    float u = this_.u0_;
    float m0, n0;
    //if(this_.hard_x)
    //{
    //  m0 = std::numeric_limits<float>::infinity();
    //}
    //else
    {
      m0 = this_.m_;
    }
    //if (other.hard_x)
    //{
    //  n0 = std::numeric_limits<float>::infinity();
    //}
    //else
    {
      n0 = other.m_;
    }

    this_.u0_ = game::Collision(this_.u0_, m0, this_.c_, other.u0_, n0, other.c_);
    other.u0_ = game::Collision(other.u0_, n0, other.c_, u, m0, this_.c_);
    this_.u_ = this_.u0_;
    other.u_ = other.u0_;
    f = Friction(this_.b_, this_.m_, this_.k_, other.b_, other.m_, other.k_);
    this_.f_ = f;
    other.f_ = f;
    
  //  this_.hard_x = (!std::isfinite(other.m_)) || other.hard_x || (!std::isfinite(this_.m_)) || this_.hard_x;
   // other.hard_x = this_.hard_x;
    this_.UpdatePosition(t);
    other.UpdatePosition(t);
    {
      this_.u_ = this_.c_ * other.c_ * this_.u_;
      other.u_ = this_.c_ * other.c_ * other.u_;
    }
  }
  else
  {

    float v = this_.v0_;
    float m0, n0;
    //if(this_.hard_y)
    //{
    //  m0 = std::numeric_limits<float>::infinity();
    //}
    //else
    {
      m0 = this_.m_;
    }
    //if(other.hard_y)
    //{
    //  n0 = std::numeric_limits<float>::infinity();
    //}
    //else
    {
      n0 = other.m_;
    }

    this_.v0_ = game::Collision(this_.v0_, m0, this_.c_, other.v0_, n0, other.c_);
    other.v0_ = game::Collision(other.v0_, n0, other.c_, v, m0, this_.c_);
    this_.v_ = this_.v0_;
    other.v_ = other.v0_; 
    g = Friction(this_.a_, this_.m_, this_.k_, other.a_, other.m_, other.k_);
    this_.g_ = g;
    other.g_ = g;
    //this_.hard_y = (!std::isfinite(other.m_)) || other.hard_y || (!std::isfinite(this_.m_)) || this_.hard_y;
    //other.hard_y = this_.hard_y;
    this_.UpdatePosition(t);
    other.UpdatePosition(t);
    this_.v_ = this_.c_ * other.c_ * this_.v_;
    other.v_ = this_.c_ * other.c_ * other.v_;
  }

  //this_.UpdateVelocity(t);
  //other.UpdateVelocity(t);
  this_.g_ = g;
  other.g_ = g;
  this_.f_ = f;
  other.f_ = f;
}

static std::pair<float, bool> CollisionTime(DynamicsImpl& this_, DynamicsImpl& other)
{
  float tx = CollisionTime(this_.x_, this_.u0_, this_.w_, other.x_, other.u0_, other.w_);
  if(tx < 0.f)
  {
    tx = std::numeric_limits<float>::infinity();
  }
  float ty = CollisionTime(this_.y_, this_.v0_, this_.h_, other.y_, other.v0_, other.h_);
  if(ty < 0.f)
  {
    ty = std::numeric_limits<float>::infinity();
  }

  std::pair<float, bool> t(std::numeric_limits<float>::infinity(), false);
  //if(std::isfinite(tx) && std::isfinite(ty))
  {
    if(tx < ty)
    {
      t.first = tx;
      t.second = true;
    }
    else
    {
      t.first = ty;
      t.second = false;
    }
  }
  if(std::isfinite(t.first) && (t.first > 1.f/60.f))
  {
    //t.first = 1.f / 60.f;
    std::cout << tx << ", " << ty << std::endl;
    //t.first = std::numeric_limits<float>::infinity();
  }
  return t;
}

typedef DynamicsImpl* DynamicsPtr;
typedef std::pair<std::pair<float, bool>, std::pair<DynamicsPtr, DynamicsPtr>> CollisionEvent;
std::priority_queue<CollisionEvent> queue;

void DynamicsImpl::Collision(DynamicsImpl& other)
{
  std::pair<float, bool> t = CollisionTime(*this, other);
  if(std::isfinite(t.first))
  {
    //game::Collision(*this, other, t.first, t.second);
    queue.push(CollisionEvent(t, std::pair<DynamicsPtr, DynamicsPtr>(this, &other)));
  }
}

bool NextCollisionQueue(void)
{
  if(queue.empty())
  {
    return true;
  }
  else
  {
    CollisionEvent& E = queue.top();
    game::Collision(E.first.first, E.first.second, *E.second.first, *E.second.second);
    //while(!queue.empty())
    {
      queue.pop();
    }
    //return false;
  }
  return queue.empty();
}

DynamicsImpl::DynamicsImpl(float x, float y, float u, float v, float w, float h, float c, float m) : x_(x), u_(u), y_(y), v_(v), w_(w), h_(h), c_(c), m_(m), t_(0), a_(0), b_(0), k_(0.f), d_(0.f), f_(0.f), g_(0.f), b(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/world.json")), 
  bb(0,0,0,0,1,1,1,0.5,0.5,0.5,b)
{
}

Dynamics::Dynamics(float x, float y, float u, float v, float w, float h, float c, float m)
{
  impl_ = std::make_shared<DynamicsImpl>(x, y, u, v, w, h, c, m);
}

void Dynamics::x(float x)
{
  impl_->x_ = x;
}

void Dynamics::y(float y)
{
  impl_->y_ = y;
}

void Dynamics::u(float u)
{
  impl_->u_ = u;
}

void Dynamics::v(float v)
{
  impl_->v_ = v;
}

void Dynamics::w(float w)
{
  impl_->w_ = w;
}

void Dynamics::h(float h)
{
  impl_->h_ = h;
}

void Dynamics::c(float c)
{
  impl_->c_ = c;
}

void Dynamics::m(float m)
{
  impl_->m_ = m;
}

void Dynamics::a(float a)
{
  impl_->a_ = a;
}

void Dynamics::b(float b)
{
  impl_->b_ = b;
}

void Dynamics::k(float k)
{
  impl_->k_ = k;
}

void Dynamics::d(float d)
{
  impl_->d_ = d;
}

float Dynamics::x(void) const
{
  return impl_->x_;
}

float Dynamics::y(void) const
{
  return impl_->y_;
}

float Dynamics::u(void) const
{
  return impl_->u_;
}

float Dynamics::v(void) const
{
  return impl_->v_;
}

float Dynamics::w(void) const
{
  return impl_->w_;
}

float Dynamics::h(void) const
{
  return impl_->h_;
}

float Dynamics::c(void) const
{
  return impl_->c_;
}

float Dynamics::m(void) const
{
  return impl_->m_;
}

float Dynamics::a(void) const
{
  return impl_->a_;
}

float Dynamics::b(void) const
{
  return impl_->b_;
}

float Dynamics::k(void) const
{
  return impl_->k_;
}

float Dynamics::d(void) const
{
  return impl_->d_;
}

void Dynamics::Step(float dt)
{
  impl_->UpdatePosition(dt);
  impl_->UpdateVelocity(dt);
  impl_->hard_x = false;
  impl_->hard_y = false;
}

void Dynamics::Collision(Dynamics const& other)
{
  impl_->Collision(*other.impl_);
}

Dynamics::operator bool(void) const
{
  return bool(impl_);
}
}