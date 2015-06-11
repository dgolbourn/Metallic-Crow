#include "saves.h"
#include <array>
#include "lua_stack.h"
#include "now.h"
#include <tuple>
#include "log.h"
#include <iostream>
#include <fstream>
namespace game
{
namespace
{
typedef std::tuple<int, int, std::string> Save;
typedef std::array<Save, 8> SaveArray;
}

class Saves::Impl
{
public:
  Impl(boost::filesystem::path const& file);
  ~Impl();

  bool Playing() const;
  int Size() const;
  int Progress(int slot) const;
  int Current(int slot) const;
  std::string LastPlayed(int slot) const;
  int LastPlayed() const;
  void Play(int slot);
  void Stop();
  void Current(int slot, int current);
  bool Check(int slot) const;
  void Save();
  
  SaveArray saves_;
  SaveArray::size_type slot_;
  boost::filesystem::path file_;
  bool playing_;
};

Saves::Impl::Impl(boost::filesystem::path const& file) : file_(file), slot_(0), playing_(false)
{
  lua::Stack lua("");
  lua.Load(file);

  int slot;
  {
    lua::Guard guard = lua.Get("last_active_save");
    lua.Pop(slot);
  }
  slot_ = SaveArray::size_type(slot);

  lua::Guard guard = lua.Get("saves");
  for (SaveArray::size_type i = 0; i < saves_.size(); ++i)
  {
    lua::Guard guard = lua.Field(i + 1);
    auto& saves = saves_[i];

    {
      lua::Guard guard = lua.Field("progress");
      lua.Pop(std::get<0>(saves));
    }

    {
      lua::Guard guard = lua.Field("current");
      lua.Pop(std::get<1>(saves));
    }

    {
      lua::Guard guard = lua.Field("last_played");
      lua.Pop(std::get<2>(saves));
    }
  }
}

void Saves::Impl::Save()
{ 
  if(playing_)
  {
    std::get<2>(saves_.at(slot_)) = calendar::Now();
  }

  std::ofstream file(file_.string());

  file << "last_active_save = " << slot_ << "\n"
          "saves = {}\n";

  for(SaveArray::size_type i = 0; i < saves_.size(); ++i)
  {
    SaveArray::size_type j = i + 1;
    auto& saves = saves_[i];
    file << "saves[" << j << "] = {}\n"
            "saves[" << j << "].progress = " << std::get<0>(saves) << "\n"
            "saves[" << j << "].current = " << std::get<1>(saves) << "\n"
            "saves[" << j << "].last_played = \"" << std::get<2>(saves) << "\"\n";
  }
}

Saves::Impl::~Impl()
{
  try
  {
    Save();
  }
  catch(...)
  {
    exception::Log("Swallowed exception");
  }
}

bool Saves::Impl::Check(int slot) const
{
  return (slot < int(saves_.size())) && (slot >= 0);
}

void Saves::Impl::Play(int slot)
{
  Stop();

  if(Check(slot))
  {
    slot_ = slot;
    playing_ = true;
  }
}

void Saves::Impl::Stop()
{
  if(playing_)
  {
    playing_ = false;
    std::get<2>(saves_.at(slot_)) = calendar::Now();
  }
}

bool Saves::Impl::Playing() const
{
  return playing_;
}

int Saves::Impl::Size() const
{
  return saves_.size();
}

int Saves::Impl::Progress(int slot) const
{
  return std::get<0>(saves_.at(slot));
}

int Saves::Impl::Current(int slot) const
{
  return std::get<1>(saves_.at(slot));
}

void Saves::Impl::Current(int slot, int current)
{
  auto& saves = saves_.at(slot);
  std::get<1>(saves) = current;
  if(std::get<1>(saves) > std::get<0>(saves))
  {
    std::get<0>(saves) = std::get<1>(saves);
  }
}

std::string Saves::Impl::LastPlayed(int slot) const
{
  return std::get<2>(saves_.at(slot));
}

int Saves::Impl::LastPlayed() const
{
  return slot_;
}

bool Saves::Playing() const
{
  return impl_->Playing();
}

int Saves::Size() const
{
  return impl_->Size();
}

int Saves::Progress(int slot) const
{
  return impl_->Progress(slot);
}

int Saves::Current(int slot) const
{
  return impl_->Current(slot);
}

std::string Saves::LastPlayed(int slot) const
{
  return impl_->LastPlayed(slot);
}

int Saves::LastPlayed() const
{
  return impl_->LastPlayed();
}

void Saves::Play(int slot)
{
  impl_->Play(slot);
}

void Saves::Stop()
{
  impl_->Stop();
}

void Saves::Current(int slot, int current)
{
  impl_->Current(slot, current);
}

Saves::operator bool() const
{
  return bool(impl_);
}

void Saves::Save()
{
  impl_->Save();
}

Saves::Saves(boost::filesystem::path const& file) : impl_(std::make_shared<Impl>(file))
{
}
}