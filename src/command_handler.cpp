#include "command_handler.h"

#include "history_handler.h"
#include "utils.h"

void process_echo(std::vector<std::string>& args) {
  int redirect_idx = args.size();
  bool isError = false;
  bool isAppend = false;
  for (int i = 0; i < (int)args.size(); i++) {
    if (args[i] == ">" || args[i] == "1>") {
      redirect_idx = i;
      break;
    }
    if (args[i] == "2>") {
      redirect_idx = i;
      isError = true;
      break;
    }
    if (args[i] == "1>>") {
      redirect_idx = i;
      isAppend = true;
      break;
    }
  }
  std::string res = "";
  for (int i = 1; i < redirect_idx; i++) {
    res += args[i];
    if (i == redirect_idx - 1)
      res += "\n";
    else
      res += " ";
  }

  if (redirect_idx == (int)args.size())
    std::cout << res;
  else if (isError) {
    if (redirect_idx + 1 >= args.size()) {
      std::cerr << "No argument for redirect found" << std::endl;
      return;
    }
    std::ofstream outputFile(args[redirect_idx + 1]);
    std::cout << res;
    outputFile << "";
  } else if (isAppend) {
    if (redirect_idx + 1 >= args.size()) {
      std::cerr << "No argument for redirect found" << std::endl;
      return;
    }
    std::ofstream outputFile(args[redirect_idx + 1], std::ios::app);
    outputFile << res;
  } else {
    if (redirect_idx + 1 >= args.size()) {
      std::cerr << "No argument for redirect found" << std::endl;
      return;
    }
    std::ofstream outputFile(args[redirect_idx + 1]);
    outputFile << res;
  }
}

void process_type(std::vector<std::string>& paths,
                  std::vector<std::string>& args) {
  std::string command = args[1];

  std::string res = "";
  if (command == "exit" || command == "type" || command == "echo" ||
      command == "pwd" || command == "history") {
    res += command + " is a shell builtin" + "\n";
    std::cout << res;
    return;
  }

  if (std::string executables_path =
          find_executables_from_paths(paths, command);
      executables_path != "") {
    res += command + " is " + executables_path + "\n";
    std::cout << res;
    return;
  }

  res += command + ": not found" + "\n";
  std::cout << res;
}

void process_change_directory(std::vector<std::string>& args) {
  // Case 1: Absolute Paths
  std::string message = "";
  if (args[1][0] == '/') {
    if (std::filesystem::exists(args[1])) {
      std::filesystem::current_path(args[1]);
    } else {
      message += "cd: " + args[1] + ": No such file or directory\n";
    }
    std::cout << message;
    return;
  }

  // Case 3: Home directory
  if (args[1][0] == '~') {
    const char* home_dir = std::getenv("HOME");
    std::filesystem::current_path(home_dir);
    return;
  }

  // Case 2: Relative Paths
  auto folders = split_directory_folders(args[1], '/');
  auto target_path = std::filesystem::current_path();

  for (auto folder : folders) {
    if (folder == ".") continue;
    if (folder == "..")
      target_path = target_path.parent_path();
    else
      target_path = target_path / folder;
  }
  if (std::filesystem::exists(target_path)) {
    std::filesystem::current_path(target_path);
  } else {
    message += "cd: " + args[1] + ": No such file or directory\n";
  }
  std::cout << message;
}

void process_history(std::vector<std::string>& hist,
                     std::vector<std::string>& args) {
  if (args.size() > 1) {
    if (args[1] == "-r") {
      read_history_from_file(hist, args[2]);
    } else if (args[1] == "-w") {
      write_history_to_file(hist, args[2]);
    } else if (args[1] == "-a") {
      append_history_to_file(hist, args[2]);
    } else {
      int num = std::stoi(args[1]);
      display_history(hist, (int)hist.size() - num);
    }
    return;
  }
  display_history(hist, 0);
}