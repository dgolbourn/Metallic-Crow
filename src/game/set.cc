#include "set.h"
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
typedef std::vector<TexturePair> Textures;

void MakeSet(json::JSON const& json, collision::Group& collision, dynamics::World& world, dynamics::Body& body, float& parallax, game::Position& position)
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
    position = body.Position();
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

class Set::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(json::JSON const& json, display::Window& window, collision::Group& dcollision, dynamics::World& world, int& plane, boost::filesystem::path const& path);
  void Init(Scene& scene, dynamics::World& world, int plane);
  void End();
  void Render(void) const;
  void Modulation(float r, float g, float b);
  dynamics::Body body_;
  Textures textures_;
  float parallax_;
  display::Modulation modulation_;
  game::Position position_;
};

void Set::Impl::Render(void) const
{
  for(auto& texture : textures_)
  {
    texture.first(display::BoundingBox(), texture.second, parallax_, false, 0., modulation_);
  }
}

void Set::Impl::End()
{
  game::Position position = body_.Position();
  for(auto& texture : textures_)
  {
    texture.second.x(texture.second.x() - position_.first + position.first);
    texture.second.y(texture.second.y() - position_.second + position.second);
  }
  position_ = position;
  modulation_ = body_.Modulation();
}

void Set::Impl::Modulation(float r, float g, float b)
{
  modulation_ = display::Modulation(r, g, b, 1.f);
}

Set::Impl::Impl(json::JSON const& json, display::Window& window, collision::Group& collision, dynamics::World& world, int& plane, boost::filesystem::path const& path)
{
  json_t* textures;
  json_t* set;
  double parallax;
  double r, g, b;
  json.Unpack("{sososisfs[fff]}",
    "set", &set,
    "textures", &textures,
    "plane", &plane,
    "parallax", &parallax,
    "modulation" , &r, &g, &b);
 
  modulation_ = display::Modulation(float(r), float(g), float(b), 1.f);
  
  MakeSet(json::JSON(set), collision, world, body_, parallax_, position_);

  for(json::JSON const& value : json::JSON(textures))
  {
    char const* page;
    json_t* render_box;
    json_t* clip;
    value.Unpack("{sssoso}",
      "page", &page,
      "clip", &clip,
      "render box", &render_box);
    textures_.emplace_back(display::Texture(display::Texture(path / page, window), display::BoundingBox(json::JSON(clip))), display::BoundingBox(json::JSON(render_box)));
  }
}

void Set::Impl::Init(Scene& scene, dynamics::World& world, int plane)
{
  scene.Add(function::Bind(&Set::Impl::Render, shared_from_this()), plane);
  if(body_)
  {
    world.End(function::Bind(&Impl::End, shared_from_this()));
  }
}

Set::Set(json::JSON const& json, display::Window& window, Scene& scene, collision::Group& collision, dynamics::World& world, boost::filesystem::path const& path)
{
  int plane;
  impl_ = std::make_shared<Impl>(json, window, collision, world, plane, path);
  impl_->Init(scene, world, plane);
}

void Set::Modulation(float r, float g, float b)
{
  impl_->Modulation(r, g, b);
}
}