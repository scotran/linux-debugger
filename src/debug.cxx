#include "debugger/debugger.hxx"

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
    ptrace(PTRACE_TRACEME, 0, 0, 0);
    execl(prog, prog, nullptr);

  } else if (pid >= 1) {
    // In parent process, execute debugger
    std::cout << "Started debugging process " << pid << std::endl;
    auto debugger = Debugger{ prog, pid };
    debugger.run();
  }
}
