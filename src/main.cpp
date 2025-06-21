#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <filesystem>

std::vector<std::string> find_path(const char*& path) {
  std::string cur = "";
  std::vector <std::string> v;
  for (int i = 0; i < strlen(path); i++) {
    if (path[i] == ':') {
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

  std::vector<std::string> paths = find_path(path);

  while (true) {
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);

    if (input == "exit 0") {
      return 0;
    } 
    
    if (input.substr(0, 4) == "type") {
      std::string command = input.substr(5, (int) input.size() - 4);
      if (command == "exit" || command == "type" || command == "echo") {
        std::cout << command << " is a shell builtin" << std::endl;
      } else if (std::string executables_path = find_executables(paths, command); executables_path != "") {
        std::cout << command << " is " << executables_path << std::endl;
      } else {
        std::cout << command << ": not found" << std::endl;
      }
    } 
    else if (input.substr(0, 4) == "echo") {
      std::cout << input.substr(5, (int) input.size() - 4) << std::endl;
    }
    else {
      std::cout << input << ": command not found" << std::endl;
    }
  }
}
