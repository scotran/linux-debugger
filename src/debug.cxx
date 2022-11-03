#include <iostream>
#include <sys/ptrace.h>
#include <unistd.h>

#include "linenoise.h"
#include "utils.hxx"

class Debugger
{
public:
  Debugger(std::string prog_name, pid_t pid)
    : prog_name_(std::move(prog_name))
    , pid_(pid)
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

auto
main(int argc, char** argv) -> int
{
  if (argc < 2) {
    std::cerr << "Program name not specified";
    return -1;
  }

  auto prog = argv[1];

  auto pid = fork();

  if (pid == 0) {
    // In child process, execute debugee
    ptrace(PTRACE_TRACEME, 0, 0, 0); // NOLINT
    execl(prog, prog, nullptr);

  } else if (pid >= 1) {
    // In parent process, execute debugger
    std::cout << "Started debugging process " << pid << std::endl;
    auto debugger = Debugger{ prog, pid };
    debugger.run();
  }
}