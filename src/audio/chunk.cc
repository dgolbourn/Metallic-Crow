#include "chunk.h"
#include "SDL_mixer.h"
#include <unordered_map>
#include "mix_exception.h"
#include "mix_library.h"
#include <vector>
#include "decoder.h"

namespace mix
{
class ChunkImpl
{
public:
  ChunkImpl(std::string const& filename);
  ~ChunkImpl(void);
  Library const mix_;
  Mix_Chunk* chunk_;
  std::vector<Uint8> data_;
};

static std::unordered_map<std::string, std::shared_ptr<ChunkImpl>> chunks;

void Free(void)
{
  chunks.clear();
}

ChunkImpl::ChunkImpl(std::string const& filename)
{
  static int const buffer_size = 4096;
  Uint8 data[buffer_size];
  ffmpeg::Decoder decoder(filename);
  while(int read = decoder.Read(data, buffer_size))
  {
    data_.insert(data_.end(), data, data + read);
  }
  chunk_ = Mix_QuickLoad_RAW(data_.data(), data_.size());
  if(!chunk_)
  {
    throw Exception();
  }
}

ChunkImpl::~ChunkImpl(void)
{
  Mix_FreeChunk(chunk_);
}

Chunk::Chunk(std::string const& filename)
{
  auto fileiter = chunks.find(filename);
  if(fileiter != chunks.end())
  {
    impl_ = fileiter->second;
  }
  else
  {
    impl_ = std::make_shared<ChunkImpl>(filename);
    chunks[filename] = impl_;
  }
}

int Chunk::Play(int repeats, int volume)
{
  int channel = Mix_PlayChannel(-1, impl_->chunk_, repeats);
  if(channel == -1)
  {
    throw Exception();
  }
  (void)Mix_Volume(channel, volume);
  return channel;
}
}