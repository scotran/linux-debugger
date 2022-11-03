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
    : prog_name_{std::move(prog_name)}
    , pid_{pid}
  {
  }

  auto run() const;
  auto handle_command(const std::string& line) const;
  auto continue_execution() const;

private:
  const std::string prog_name_;
  const pid_t pid_;
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
