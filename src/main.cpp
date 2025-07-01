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

int main() {
  enableRawMode();

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
    read_input_per_char(input, paths, hist, hist_idx);

    // Push the input to history
    hist.push_back(input);
    hist_idx++;

    // Split inputs into arguments 
    std::vector<std::string> args = split_shell_inputs(input);

    // Handle commands
    if (args[0] == "exit") {
      break;
    }
    if (args[0] == "type") {
      process_type(paths, args);
      continue;
    }
    if (args[0] == "echo") {
      process_echo(args);
      continue;
    }
    if (args[0] == "pwd") {
      std::cout << std::filesystem::current_path().c_str() << std::endl;
      continue;
    }
    if (args[0] == "cd") {
      process_change_directory(args);
      continue;
    }
    if (args[0] == "history") {
      process_history(hist, args);
      continue;
    }
    if (std::string executables_path =
            find_executables_from_paths(paths, args[0]);
        executables_path != "") {
      system(input.c_str());
      continue;
    }
    std::cout << input << ": command not found" << std::endl;
  }

  if (hist_path) write_history_to_file(hist, hist_path);
}
