#include "body.h"
#include "collision_group.h"
namespace game
{
dynamics::Body MakeBody(json::JSON const& json, dynamics::World& world, collision::Group& collision);
}