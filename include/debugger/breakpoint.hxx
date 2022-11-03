#pragma once

#include <cstdint>
#include <unistd.h>

class Breakpoint
{
public:
  Breakpoint(pid_t pid, std::intptr_t addr)
    : pid_{ pid }
    , addr_{ addr }
  {
  }

  auto enable();
  auto disable();

  auto is_enabled() const -> bool;
  auto get_address() const -> std::intptr_t;

private:
  pid_t pid_;
  std::intptr_t addr_;
  bool enabled_{};
  uint8_t saved_data_{}; // data that used to be at breakpoint address
};

auto
Breakpoint::enable()
{
  auto data = ptrace(PTRACE_PEEKDATA, pid_, addr_, 0);
  saved_data_ = static_cast<uint8_t>(data & 0xff); // save bottom byte
  uint64_t int3 = 0xcc;
  uint64_t data_with_int3 =
    ((data & ~0xff) | int3); // clear bottom byte and set bottom byte to 0xcc
  ptrace(PTRACE_POKEDATA, pid_, addr_, data_with_int3);

  enabled_ = true;
}

auto
Breakpoint::disable()
{
  auto data = ptrace(PTRACE_PEEKDATA, pid_, addr_, 0);
  auto restored_data = ((data & ~0xff) | saved_data_);
  ptrace(PTRACE_POKEDATA, pid_, addr_, restored_data);

  enabled_ = false;
}