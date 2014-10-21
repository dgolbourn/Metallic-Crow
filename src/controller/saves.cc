#include "saves.h"
#include <array>
#include "json.h"
#include "now.h"
#include <tuple>
#include "log.h"
#include <iostream>
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
  Impl(std::string const& file);
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
  
  SaveArray progress_;
  SaveArray::size_type slot_;
  std::string file_;
  bool playing_;
};

Saves::Impl::Impl(std::string const& file) : file_(file), slot_(0), playing_(false)
{
  char const* last_played[8];
  json::JSON json(file);
  
  json.Unpack("{s[{sisiss}{sisiss}{sisiss}{sisiss}{sisiss}{sisiss}{sisiss}{sisiss}]si}",
    "saves",
    "progress", &std::get<0>(progress_[0]), "current", &std::get<1>(progress_[0]), "last played", &last_played[0],
    "progress", &std::get<0>(progress_[1]), "current", &std::get<1>(progress_[1]), "last played", &last_played[1],
    "progress", &std::get<0>(progress_[2]), "current", &std::get<1>(progress_[2]), "last played", &last_played[2],
    "progress", &std::get<0>(progress_[3]), "current", &std::get<1>(progress_[3]), "last played", &last_played[3],
    "progress", &std::get<0>(progress_[4]), "current", &std::get<1>(progress_[4]), "last played", &last_played[4],
    "progress", &std::get<0>(progress_[5]), "current", &std::get<1>(progress_[5]), "last played", &last_played[5],
    "progress", &std::get<0>(progress_[6]), "current", &std::get<1>(progress_[6]), "last played", &last_played[6],
    "progress", &std::get<0>(progress_[7]), "current", &std::get<1>(progress_[7]), "last played", &last_played[7],
    "last active save", &slot_);

  for(SaveArray::size_type i = 0; i < progress_.size(); ++i)
  {
    std::get<2>(progress_[i]) = std::string(last_played[i]);
  }
}

void Saves::Impl::Save()
{
  if(playing_)
  {
    std::get<2>(progress_.at(slot_)) = calendar::Now();
  }

  json::JSON("{s[{sisiss}{sisiss}{sisiss}{sisiss}{sisiss}{sisiss}{sisiss}{sisiss}]si}",
    "saves",
    "progress", std::get<0>(progress_[0]), "current", std::get<1>(progress_[0]), "last played", std::get<2>(progress_[0]).c_str(),
    "progress", std::get<0>(progress_[1]), "current", std::get<1>(progress_[1]), "last played", std::get<2>(progress_[1]).c_str(),
    "progress", std::get<0>(progress_[2]), "current", std::get<1>(progress_[2]), "last played", std::get<2>(progress_[2]).c_str(),
    "progress", std::get<0>(progress_[3]), "current", std::get<1>(progress_[3]), "last played", std::get<2>(progress_[3]).c_str(),
    "progress", std::get<0>(progress_[4]), "current", std::get<1>(progress_[4]), "last played", std::get<2>(progress_[4]).c_str(),
    "progress", std::get<0>(progress_[5]), "current", std::get<1>(progress_[5]), "last played", std::get<2>(progress_[5]).c_str(),
    "progress", std::get<0>(progress_[6]), "current", std::get<1>(progress_[6]), "last played", std::get<2>(progress_[6]).c_str(),
    "progress", std::get<0>(progress_[7]), "current", std::get<1>(progress_[7]), "last played", std::get<2>(progress_[7]).c_str(),
    "last active save", slot_).Write(file_);
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
  return (slot < int(progress_.size())) && (slot >= 0);
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
    std::get<2>(progress_.at(slot_)) = calendar::Now();
  }
}

bool Saves::Impl::Playing() const
{
  return playing_;
}

int Saves::Impl::Size() const
{
  return progress_.size();
}

int Saves::Impl::Progress(int slot) const
{
  return std::get<0>(progress_.at(slot));
}

int Saves::Impl::Current(int slot) const
{
  return std::get<1>(progress_.at(slot));
}

void Saves::Impl::Current(int slot, int current)
{
  std::get<1>(progress_.at(slot)) = current;
  if(std::get<1>(progress_.at(slot)) > std::get<0>(progress_.at(slot)))
  {
    std::get<0>(progress_.at(slot)) = std::get<1>(progress_.at(slot));
  }
}

std::string Saves::Impl::LastPlayed(int slot) const
{
  return std::get<2>(progress_.at(slot));
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

Saves::Saves(std::string const& file) : impl_(std::make_shared<Impl>(file))
{
}
}