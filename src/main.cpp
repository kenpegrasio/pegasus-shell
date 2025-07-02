#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "autocompletion_handler.h"
#include "command_handler.h"
#include "history_handler.h"
#include "input_handler.h"
#include "utils.h"

void process_command(std::vector<std::string>& args,
                     std::vector<std::string>& paths,
                     std::vector<std::string>& hist, std::string& input,
                     bool& exited) {
  // Handle commands
  if (args[0] == "exit") {
    exited = true;
    return;
  }
  if (args[0] == "type") {
    process_type(paths, args);
    return;
  }
  if (args[0] == "echo") {
    process_echo(args);
    return;
  }
  if (args[0] == "pwd") {
    std::cout << std::filesystem::current_path().c_str() << std::endl;
    return;
  }
  if (args[0] == "cd") {
    process_change_directory(args);
    return;
  }
  if (args[0] == "history") {
    process_history(hist, args);
    return;
  }
  if (std::string executables_path =
          find_executables_from_paths(paths, args[0]);
      executables_path != "") {
    system(input.c_str());
    return;
  }
  std::cout << input << ": command not found" << std::endl;
}

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Fetch path from environment
  const char* path = std::getenv("PATH");  // may return nullptr if it not exist
  if (!path) {
    std::cout << "PATH is not set" << std::endl;
    return 1;
  }
  std::vector<std::string> paths = split_env_paths(path, ':');

  // Fetch history from environment
  std::vector<std::string> hist;
  const char* hist_path = std::getenv("HISTFILE");
  if (hist_path) read_history_from_file(hist, hist_path);
  int hist_idx = hist.size();

  while (true) {
    std::cout << "$ ";

    // Read input character by character
    std::string input = "";
    enableRawMode();
    read_input_per_char(input, paths, hist, hist_idx);
    disableRawMode();
    // Make sure to only call raw mode when input. If not, child processes (in
    // the fork), may not behave as expected

    // Push the input to history
    hist.push_back(input);
    hist_idx++;

    // Split inputs into arguments
    std::vector<std::string> raw_args = split_shell_inputs(input);
    raw_args.push_back("|");

    auto splitted_args = split_arguments_for_pipeline(raw_args, "|");
    bool exited = false;

    if (splitted_args.size() == 1) {
      for (auto args : splitted_args) {
        process_command(args, paths, hist, input, exited);
      }
      if (exited) break;
      continue;
    }

    for (int i = 1; i < (int)splitted_args.size(); i++) {
      int pipefd[2];
      pid_t pid1, pid2;
      if (pipe(pipefd) == -1) {
        perror("pipe");
        break;
      }
      if ((pid1 = fork()) == 0) {
        bool exited = false;
        close(pipefd[0]);  // close ununsed end to avoid deadlocks, hangs, and
                           // file descriptor leaks
        dup2(pipefd[1],
             STDOUT_FILENO);  // redirect stdout to write into the pipe
        close(pipefd[1]);
        process_command(splitted_args[i - 1], paths, hist, input, exited);
        exit(1);  // command processing is done, so exit from the fork and back
                  // to parent, i.e., main()
      }
      if ((pid2 = fork()) == 0) {
        bool exited = false;
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        process_command(splitted_args[i], paths, hist, input, exited);
        exit(1);
      }

      // Close both ends of pipe
      close(pipefd[0]);
      close(pipefd[1]);

      // Wait for both processes to complete
      waitpid(pid1, nullptr, 0);
      waitpid(pid2, nullptr, 0);
    }
  }

  if (hist_path) write_history_to_file(hist, hist_path);
}
