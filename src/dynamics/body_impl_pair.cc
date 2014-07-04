#include "body_impl_pair.h"
namespace dynamics
{
BodyImplPair Make(BodyImpl* body_a, BodyImpl* body_b)
{
  BodyImplPair ret;
  if(body_a < body_b)
  {
    ret = BodyImplPair(body_a, body_b);
  }
  else
  {
    ret = BodyImplPair(body_b, body_a);
  }
  return ret;
}
}
