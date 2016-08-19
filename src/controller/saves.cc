#include "saves.h"
#include <array>
#include "lua_stack.h"
#include "now.h"
#include <tuple>
#include "log.h"
#include <iostream>
#include <fstream>
namespace
{
typedef std::tuple<int, int, std::string> Save;
typedef std::array<Save, 8> SaveArray;
}

namespace game
{
class Saves::Impl
{
public:
  Impl(boost::filesystem::path const& file);
  ~Impl();
  auto Playing() const ->  bool;
  auto Size() const -> int;
  auto Progress(int slot) const -> int;
  auto Current(int slot) const -> int;
  auto LastPlayed(int slot) const -> std::string;
  auto LastPlayed() const -> int;
  auto Play(int slot) -> void;
  auto Stop() -> void;
  auto Current(int slot, int current) -> void;
  auto Check(int slot) const -> bool;
  auto Save() -> void;
  auto Delete(int slot) ->void;
  SaveArray::size_type slot_;
  SaveArray saves_;
  boost::filesystem::path file_;
  bool playing_;
};

Saves::Impl::Impl(boost::filesystem::path const& file) : file_(file), playing_(false)
{
  lua::Stack lua("");
  lua.Load(file);

  int slot;
  {
    lua::Guard guard = lua.Get("last_active_save");
    lua.Pop(slot);
  }
  slot_ = static_cast<SaveArray::size_type>(slot);

  lua::Guard guard = lua.Get("saves");
  for(SaveArray::size_type i = 0; i < saves_.size(); ++i)
  {
    lua::Guard guard = lua.Field(i + 1);
    auto& saves = saves_[i];
    std::get<0>(saves) = lua.Field<int>("progress");
    std::get<1>(saves) = lua.Field<int>("current");
    std::get<2>(saves) = lua.Field<std::string>("last_played");
  }
}

auto Saves::Impl::Save() -> void
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

auto Saves::Impl::Check(int slot) const -> bool
{
  return (slot < static_cast<int>(saves_.size())) && (slot >= 0);
}

auto Saves::Impl::Play(int slot) -> void
{
  Stop();

  if(Check(slot))
  {
    slot_ = slot;
    playing_ = true;
  }
}

auto Saves::Impl::Stop() -> void
{
  if(playing_)
  {
    playing_ = false;
    std::get<2>(saves_.at(slot_)) = calendar::Now();
  }
}

auto Saves::Impl::Playing() const -> bool
{
  return playing_;
}

auto Saves::Impl::Size() const -> int
{
  return saves_.size();
}

auto Saves::Impl::Progress(int slot) const -> int
{
  return std::get<0>(saves_.at(slot));
}

auto Saves::Impl::Current(int slot) const -> int
{
  return std::get<1>(saves_.at(slot));
}

auto Saves::Impl::Current(int slot, int current) -> void
{
  auto& saves = saves_.at(slot);
  std::get<1>(saves) = current;
  if(std::get<1>(saves) > std::get<0>(saves))
  {
    std::get<0>(saves) = std::get<1>(saves);
  }
}

auto Saves::Impl::LastPlayed(int slot) const -> std::string
{
  return std::get<2>(saves_.at(slot));
}

auto Saves::Impl::LastPlayed() const -> int
{
  return slot_;
}

auto Saves::Impl::Delete(int slot) -> void
{
  auto& saves = saves_.at(slot);
  std::get<0>(saves) = 0;
  std::get<1>(saves) = 0;
  std::get<2>(saves) = "";
}

auto Saves::Playing() const -> bool
{
  return impl_->Playing();
}

auto Saves::Size() const -> int
{
  return impl_->Size();
}

auto Saves::Progress(int slot) const -> int
{
  return impl_->Progress(slot);
}

auto Saves::Current(int slot) const -> int
{
  return impl_->Current(slot);
}

auto Saves::LastPlayed(int slot) const -> std::string
{
  return impl_->LastPlayed(slot);
}

auto Saves::LastPlayed() const -> int
{
  return impl_->LastPlayed();
}

auto Saves::Play(int slot) -> void
{
  impl_->Play(slot);
}

auto Saves::Stop() -> void
{
  impl_->Stop();
}

auto Saves::Current(int slot, int current) -> void
{
  impl_->Current(slot, current);
}

Saves::operator bool() const
{
  return static_cast<bool>(impl_);
}

auto Saves::Save() -> void
{
  impl_->Save();
}

auto Saves::Delete(int slot) -> void
{
  impl_->Delete(slot);
}

Saves::Saves(boost::filesystem::path const& file) : impl_(std::make_shared<Impl>(file))
{
}
}