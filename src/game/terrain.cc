#include "terrain.h"
#include "bounding_box.h"
#include "body.h"
#include "bind.h"
#include <vector>
#include "texture.h"
#include "json_iterator.h"
namespace game
{
class TerrainImpl final : public std::enable_shared_from_this<TerrainImpl>
{
public:
  TerrainImpl(json::JSON const& json, display::Window& window, DynamicsCollision& dcollision, dynamics::World& world, int& plane);
  void Init(Scene& scene, int plane);
  void Render(void) const;
  dynamics::Body body_;
  typedef std::pair<display::Texture, display::BoundingBox> TexturePair;
  std::vector<TexturePair> textures_;
  float parallax_;
};

void TerrainImpl::Render(void) const
{
  for(auto& texture : textures_)
  {
    texture.first(display::BoundingBox(), texture.second, parallax_, false, 0.);
  }
}

TerrainImpl::TerrainImpl(json::JSON const& json, display::Window& window, DynamicsCollision& dcollision, dynamics::World& world, int& plane)
{
  json_t* textures;
  json_t* body_ptr;
  double parallax;
  
  json.Unpack("{sososisf}",
    "body", &body_ptr,
    "textures", &textures,
    "z", &plane,
    "parallax", &parallax);
 
  parallax_ = float(parallax);

  if(auto body = json::JSON(body_ptr))
  {
    body_ = dynamics::Body(body, world);
    dcollision.Add(dynamics::Type::Body, body_);
  }

  textures_ = std::vector<TexturePair>(json::JSON(textures).Size());
  auto texture_iter = textures_.begin();
  for(json::JSON const& value : json::JSON(textures))
  {
    char const* image;
    json_t* render_box;
    value.Unpack("{ssso}",
      "image", &image,
      "render box", &render_box);
    *texture_iter++ = TexturePair(display::Texture(image, window), display::BoundingBox(json::JSON(render_box)));
  }
}

void TerrainImpl::Init(Scene& scene, int plane)
{
  scene.Add(event::Bind(&TerrainImpl::Render, shared_from_this()), plane);
}

Terrain::Terrain(json::JSON const& json, display::Window& window, Scene& scene, DynamicsCollision& dcollision, dynamics::World& world)
{
  int plane;
  impl_ = std::make_shared<TerrainImpl>(json, window, dcollision, world, plane);
  impl_->Init(scene, plane);
}
}