#include "prop.h"
#include "bounding_box.h"
#include "body.h"
#include "bind.h"
#include <vector>
#include "texture.h"
#include "json_iterator.h"
#include "collision_group.h"
#include "make_body.h"
#include "exception.h"
namespace game
{
namespace
{
typedef std::pair<display::Texture, display::BoundingBox> TexturePair;

void MakeProp(json::JSON const& json, collision::Group& collision, dynamics::World& world, dynamics::Body& body, float& parallax)
{
  char const* type_ptr;
  json_t* properties;
  json.Unpack("{ssso}",
    "type", &type_ptr,
    "properties", &properties);

  std::string type(type_ptr);

  if(type == "body")
  {
    body = MakeBody(json::JSON(properties), world, collision);
    parallax = 1.f;
  }
  else if(type == "parallax")
  {
    double parallax;
    json::JSON(properties).Unpack("f", &parallax);
    parallax = float(parallax);
  }
  else
  {
    BOOST_THROW_EXCEPTION(exception::Exception());
  }
}
}

class Prop::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(json::JSON const& json, display::Window& window, collision::Group& dcollision, dynamics::World& world, int& plane);
  void Init(Scene& scene, int plane);
  void Render(void) const;
  void Modulation(float r, float g, float b);
  dynamics::Body body_;
  std::vector<TexturePair> textures_;
  float parallax_;
  display::Modulation modulation_;
};

void Prop::Impl::Render(void) const
{
  for(auto& texture : textures_)
  {
    texture.first(display::BoundingBox(), texture.second, parallax_, false, 0., modulation_);
  }
}

void Prop::Impl::Modulation(float r, float g, float b)
{
  modulation_ = display::Modulation(r, g, b, 1.f);
}

Prop::Impl::Impl(json::JSON const& json, display::Window& window, collision::Group& collision, dynamics::World& world, int& plane)
{
  json_t* textures;
  json_t* prop;
  double parallax;
  double r, g, b;
  json.Unpack("{sososisfs[fff]}",
    "prop", &prop,
    "textures", &textures,
    "plane", &plane,
    "parallax", &parallax,
    "modulation" , &r, &g, &b);
 
  modulation_ = display::Modulation(float(r), float(g), float(b), 1.f);
  
  MakeProp(json::JSON(prop), collision, world, body_, parallax_);

  for(json::JSON const& value : json::JSON(textures))
  {
    char const* page;
    json_t* render_box;
    json_t* clip;
    value.Unpack("{sssoso}",
      "page", &page,
      "clip", &clip,
      "render box", &render_box);
    textures_.emplace_back(display::Texture(display::Texture(page, window), display::BoundingBox(json::JSON(clip))), display::BoundingBox(json::JSON(render_box)));
  }
}

void Prop::Impl::Init(Scene& scene, int plane)
{
  scene.Add(function::Bind(&Prop::Impl::Render, shared_from_this()), plane);
}

Prop::Prop(json::JSON const& json, display::Window& window, Scene& scene, collision::Group& collision, dynamics::World& world)
{
  int plane;
  impl_ = std::make_shared<Prop::Impl>(json, window, collision, world, plane);
  impl_->Init(scene, plane);
}

void Prop::Modulation(float r, float g, float b)
{
  impl_->Modulation(r, g, b);
}
}