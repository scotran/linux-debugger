#include "debugger/debugger.hxx"

#include <sys/personality.h>
#include <sys/ptrace.h>
#include <unistd.h>

auto
main(int argc, char** argv) -> int
{
  if (argc < 2) {
    std::cerr << "Error: Program name not specified." << std::endl;
    return -1;
  }

  auto prog = argv[1];

  auto pid = fork();

  if (pid == -1) {
    std::cerr << "Error: Unable to create new process for debugging."
              << std::endl;
    return -1;
  }

  if (pid == 0) {
    // In child process, execute debugee
    personality(ADDR_NO_RANDOMIZE);
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
      std::cerr << "Error: Unable while starting to trace program."
                << std::endl;
    }

    execl(prog, prog, nullptr);
    std::cerr << "Error while trying to execute program." << std::endl;
    return -1;
  } else if (pid >= 1) {
    // In parent process, execute debugger
    std::cout << "Started debugging process " << pid << std::endl;
    auto debugger = Debugger{ prog, pid };
    debugger.run();
  }
}
