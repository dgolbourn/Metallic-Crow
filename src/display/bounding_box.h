#ifndef BOUNDING_BOX_H_
#define BOUNDING_BOX_H_

#include <memory>

namespace display
{

class BoundingBox
{
  friend class TextureImpl;
public:
  BoundingBox(void);
  BoundingBox(int x, int y, int w, int h);
  BoundingBox(BoundingBox const& other);
  BoundingBox(BoundingBox&& other);
  ~BoundingBox(void);
  BoundingBox& operator=(BoundingBox other);

  /**Return true if both bounding boxes are identical.*/
  bool operator ==(const BoundingBox& compare) const;

  /**Return the intersection of the two bounding boxes.*/
  BoundingBox operator |(const BoundingBox& other);

  /**Return the smallest bounding box that contains both boxes.*/
  BoundingBox operator &(const BoundingBox& other);

  /**Return true if the two bounding boxes overlap.*/
  bool operator &&(const BoundingBox& other);

  /**Return true if the bounding box has no extent.*/
  bool Empty(void);
private:
  std::shared_ptr<class BoundingBoxImpl> impl_;
};

}
#endif