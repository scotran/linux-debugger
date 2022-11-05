#pragma once

#include <cstdint>
#include <sys/ptrace.h>
#include <unistd.h>
#include <errno.h>

class Breakpoint
{
public:
  Breakpoint()
    : pid_{}
    , addr_{}
  {
  }

  Breakpoint(pid_t pid, std::intptr_t addr)
    : pid_{ pid }
    , addr_{ addr }
  {
  }

  auto enable() -> bool;
  auto disable() -> bool;

  auto is_enabled() const -> bool;
  auto get_address() const -> std::intptr_t;

private:
  pid_t pid_;
  std::intptr_t addr_;
  bool enabled_{};
  uint8_t saved_data_{}; // data that used to be at breakpoint address
};

auto
Breakpoint::enable() -> bool
{
  if (enabled_)
    return true;

  errno = 0;
  auto data = ptrace(PTRACE_PEEKDATA, pid_, addr_, 0);

  if (errno != 0)
    return false;

  saved_data_ = static_cast<uint8_t>(data & 0xff); // save bottom byte
  uint64_t int3 = 0xcc;
  uint64_t data_with_int3 =
    ((data & ~0xff) | int3); // clear bottom byte and set bottom byte to 0xcc

  if (ptrace(PTRACE_POKEDATA, pid_, addr_, data_with_int3) < 0) {
    return false;
  }

  enabled_ = true;
  return true;
}

auto
Breakpoint::disable() -> bool
{
  if (!enabled_)
    return true;

  errno = 0;
  auto data = ptrace(PTRACE_PEEKDATA, pid_, addr_, 0);

  if (errno != 0)
    return false;

  auto restored_data = ((data & ~0xff) | saved_data_);
  if (ptrace(PTRACE_POKEDATA, pid_, addr_, restored_data) < 0)
    return false;

  enabled_ = false;
  return true;
}
