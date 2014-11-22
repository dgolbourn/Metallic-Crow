#include "make_body.h"
#include "json_iterator.h"
namespace game
{ 
dynamics::Body MakeBody(json::JSON const& json, dynamics::World& world, collision::Group& collision)
{
  dynamics::Body body;
  if(json)
  {
    json_t* names;
    json_t* body_ptr;
    json.Unpack("{soso}",
      "names", &names,
      "body", &body_ptr);

    body = dynamics::Body(json::JSON(body_ptr), world);

    for(json::JSON name_ptr : json::JSON(names))
    {
      char const* name;
      name_ptr.Unpack("s", &name);
      collision.Add(name, body);
    }
  }
  return body;
}
}