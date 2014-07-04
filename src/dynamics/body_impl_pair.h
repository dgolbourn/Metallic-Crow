#ifndef BODY_IMPL_PAIR_H_
#define BODY_IMPL_PAIR_H_
#include <utility>
#include "body_impl.h"
namespace dynamics
{
typedef std::pair<BodyImpl*, BodyImpl*> BodyImplPair;
BodyImplPair Make(BodyImpl* body_a, BodyImpl* body_b);
}
#endif