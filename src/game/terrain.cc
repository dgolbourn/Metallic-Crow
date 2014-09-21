#include "terrain.h"
#include "bounding_box.h"
#include "body.h"
#include "bind.h"
#include <vector>
#include "texture.h"
#include "json_iterator.h"
#include "collision_group.h"
#include "make_body.h"
namespace game
{
namespace
{
typedef std::pair<display::Texture, display::BoundingBox> TexturePair;
}

class Terrain::Impl final : public std::enable_shared_from_this<Impl>
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

void Terrain::Impl::Render(void) const
{
  for(auto& texture : textures_)
  {
    texture.first(display::BoundingBox(), texture.second, parallax_, false, 0., modulation_);
  }
}

void Terrain::Impl::Modulation(float r, float g, float b)
{
  modulation_ = display::Modulation(r, g, b, 1.f);
}

Terrain::Impl::Impl(json::JSON const& json, display::Window& window, collision::Group& collision, dynamics::World& world, int& plane)
{
  json_t* textures;
  json_t* body;
  double parallax;
  double r, g, b;
  json.Unpack("{sososisfs[fff]}",
    "body", &body,
    "textures", &textures,
    "z", &plane,
    "parallax", &parallax,
    "modulation" , &r, &g, &b);
 
  modulation_ = display::Modulation(float(r), float(g), float(b), 1.f);
  parallax_ = float(parallax);

  body_ = MakeBody(json::JSON(body), world, collision);

  for(json::JSON const& value : json::JSON(textures))
  {
    char const* image;
    json_t* render_box;
    json_t* clip;
    value.Unpack("{sssoso}",
      "image", &image,
      "render box", &render_box,
      "clip", &clip);
    textures_.emplace_back(display::Texture(display::Texture(image, window), display::BoundingBox(json::JSON(clip))), display::BoundingBox(json::JSON(render_box)));
  }
}

void Terrain::Impl::Init(Scene& scene, int plane)
{
  scene.Add(function::Bind(&Terrain::Impl::Render, shared_from_this()), plane);
}

Terrain::Terrain(json::JSON const& json, display::Window& window, Scene& scene, collision::Group& collision, dynamics::World& world)
{
  int plane;
  impl_ = std::make_shared<Terrain::Impl>(json, window, collision, world, plane);
  impl_->Init(scene, plane);
}

void Terrain::Modulation(float r, float g, float b)
{
  impl_->Modulation(r, g, b);
}
}