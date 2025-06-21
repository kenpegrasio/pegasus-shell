#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <filesystem>

std::vector<std::string> split_inputs(std::string input, char delimiter) {
  std::string cur = "";
  std::vector <std::string> v;
  for (int i = 0; i < (int) input.size(); i++) {
    if (input[i] == delimiter) {
      v.push_back(cur);
      cur = "";
    } else {
      cur += input[i];
    }
  }
  v.push_back(cur);
  return v;
}

std::vector<std::string> split_paths(const char*& path, char delimiter) {
  std::string cur = "";
  std::vector <std::string> v;
  for (int i = 0; i < strlen(path); i++) {
    if (path[i] == delimiter) {
      v.push_back(cur);
      cur = "";
    } else {
      cur += path[i];
    }
  }
  v.push_back(cur);
  return v;
}

std::vector<std::string> split_folders(std::string path, char delimiter) {
  std::string cur = "";
  std::vector <std::string> v;
  for (int i = 0; i < (int) path.size(); i++) {
    if (path[i] == delimiter) {
      v.push_back(cur);
      cur = "";
    } else {
      cur += path[i];
    }
  }
  v.push_back(cur);
  return v;
}

std::string find_executables(std::vector<std::string>& paths, std::string& command) {
  for (auto path : paths) {
    std::string full_path = path + "/" + command;
    try {
      for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path() == full_path) {
          return full_path;
        }
      }
    } catch (const std::filesystem::filesystem_error& e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
  }
  return "";
}

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  const char* path = std::getenv("PATH"); // may return nullptr if it doesn't exist

  if (!path) {
    std::cout << "PATH is not set" << std::endl;
    return 1;
  }

  std::vector<std::string> paths = split_paths(path, ':');

  while (true) {
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);
    std::vector<std::string> args = split_inputs(input, ' ');

    if (args[0] == "exit") {
      return 0;
    } 
    
    if (args[0] == "type") {
      std::string command = args[1];
      if (command == "exit" || command == "type" || command == "echo" || command == "pwd") {
        std::cout << command << " is a shell builtin" << std::endl;
      } else if (std::string executables_path = find_executables(paths, command); executables_path != "") {
        std::cout << command << " is " << executables_path << std::endl;
      } else {
        std::cout << command << ": not found" << std::endl;
      }
      continue;
    } 
    
    if (args[0] == "echo") {
      std::cout << input.substr(5, (int) input.size() - 4) << std::endl;
      continue;
    }

    if (args[0] == "pwd") {
      std::cout << std::filesystem::current_path().c_str() << std::endl;
      continue;
    }

    if (args[0] == "cd") {
      // Case 1: Absolute Paths
      if (args[1][0] == '/') {
        if (std::filesystem::exists(args[1])) {
          std::filesystem::current_path(args[1]);
        } else {
          std::cout << "cd: " << args[1] << ": No such file or directory" << std::endl;
        }
        continue;
      }

      // Case 2: Relative Paths
      auto folders = split_folders(args[1], '/');
      auto target_path = std::filesystem::current_path();
      
      for (auto folder : folders) {
        if (folder == ".") continue;
        if (folder == "..") target_path = target_path.parent_path();
        else target_path = target_path / folder;
      }
      if (std::filesystem::exists(target_path)) {
        std::filesystem::current_path(target_path);
        continue;
      } else {
        std::cout << "cd: " << args[1] << ": No such file or directory" << std::endl;
      }

      continue;
    }

    if (std::string executables_path = find_executables(paths, args[0]); executables_path != "") {
      system(input.c_str());
      continue;
    }
    
    std::cout << input << ": command not found" << std::endl;
  }
}
