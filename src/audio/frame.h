#ifndef FRAME_H_
#define FRAME_H_

extern "C"
{
#include "libavutil/frame.h"
}
#include <memory>

namespace ffmpeg
{
class Frame
{
public:
  Frame(void);
  AVFrame* operator->(void) const;
  operator AVFrame* (void) const;
  void Clear(void);
  uint8_t const** data(void) const;

  Frame(Frame const& other);
  Frame(Frame&& other);
  Frame& operator=(Frame other);
  ~Frame(void);
private:
  std::shared_ptr<AVFrame> frame_;
};
}
#endif