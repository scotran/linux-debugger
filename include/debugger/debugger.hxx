#pragma once

#include "linenoise.h"
#include "utils.hxx"

#include <iostream>
#include <string>
#include <unistd.h>

class Debugger
{
public:
  Debugger(std::string prog_name, pid_t pid)
    : prog_name_{ std::move(prog_name) }
    , pid_{ pid }
  {
  }

  auto run() const;
  auto handle_command(const std::string& line) const;
  auto continue_execution() const;
  auto set_breakpoint_at_address();

private:
  const std::string prog_name_;
  const pid_t pid_;
  std::unordered_map<std::intptr_t, Breakpoint> breakpoints_;
};

auto
Debugger::continue_execution() const
{
  ptrace(PTRACE_CONT, pid_, 0, 0);

  int wait_status;
  waitpid(pid_, &wait_status, 0);
}

auto
Debugger::handle_command(const std::string& line) const
{
  auto args = split(line, " ");
  auto command = args[0];

  if (is_prefix(command, "continue")) {
    continue_execution();
  } else if (is_prefix(command, "break")) {
    std::string addr{ args[1], 2 }; // assume user has written 0xADDRESS
    set_breakpoint_at_address(std::stoi(addr, 0, 16));
  } else {
    std::cerr << "Unknown command\n";
  }
}

auto
Debugger::run() const
{
  int wait_status;
  waitpid(pid_, &wait_status, 0);

  char* line = nullptr;
  while ((line = linenoise("debug> ")) != nullptr) {
    handle_command(line);
    linenoiseHistoryAdd(line);
    linenoiseFree(line);
  }
}

auto
Debugger::set_breakpoint_at_address(std::intptr addr)
{
  std::cout << "Set breakpoint at address 0x" << std::hex << addr << std::endl;
  if (auto it = breakpoints_.find(addr); it != breakpoints_.end()) {
    it->second.enable();
  } else {
    breakpoints_.try_emplace(addr, pid_, addr);
    breakpoints_[addr].enable();
  }
}
