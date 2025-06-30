#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <filesystem>
#include <termios.h>
#include <unistd.h>
#include <set>

std::vector<std::string> split_inputs(std::string input) {
  std::string cur = "";
  std::vector <std::string> v;
  bool inside_single_quote = false;
  bool inside_double_quote = false;
  for (int i = 0; i < (int) input.size(); i++) {
    if (!inside_single_quote && input[i] == '\"') {
      inside_double_quote = !inside_double_quote;
      continue;
    }
    if (!inside_double_quote && input[i] == '\'') {
      inside_single_quote = !inside_single_quote;
      continue;
    }
    if (input[i] == '\\') {
      if (inside_single_quote) {
        /*
          Enclosing characters in single quotes (‘'’) preserves the literal value of each character within the quotes. 
          A single quote may not occur between single quotes, even when preceded by a backslash.
          Source: https://www.gnu.org/software/bash/manual/bash.html#Single-Quotes
        */
        cur += input[i];
      } else if (inside_double_quote) {
        /*
          The backslash retains its special meaning only when followed by one of the following characters: ‘$’, ‘`’, ‘"’, ‘\’, or newline. 
          Within double quotes, backslashes that are followed by one of these characters are removed. 
          Backslashes preceding characters without a special meaning are left unmodified.
          Source: https://www.gnu.org/software/bash/manual/bash.html#Double-Quotes 
          Note: In C++ '\n' is treated as a single character
        */
        bool isSpecialCharacter = (
          input[i + 1] == '\n' || input[i + 1] == '$' || 
          input[i + 1] == '\"' || input[i + 1] == '`'  || 
          input[i + 1] == '\\'
        );

        if (i + 1 < input.length() && isSpecialCharacter) {
          cur += input[i + 1];
          i++;
        } else {
          cur += input[i];
        }
      } else {
        /*
          A non-quoted backslash ‘\’ is the Bash escape character. It preserves the literal value of the next character that follows. 
          Source: https://www.gnu.org/software/bash/manual/bash.html#Escape-Character 
        */
        if (i + 1 < input.length()) {
          cur += input[i + 1];
          i++;
        } else {
          throw std::invalid_argument("\\ must be followed by a character");
        }
      }
      continue;
    }

    if (!inside_double_quote && !inside_single_quote && input[i] == ' ') {
      if (cur == "") continue;
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

std::set<std::string> generate_executables(std::vector<std::string>& paths) {
  std::set<std::string> res;
  for (const auto& path : paths) {
    try {
      for (const auto& entry: std::filesystem::directory_iterator(path)) {
        res.insert(entry.path().filename().string());
      }
    } catch (const std::filesystem::filesystem_error& e) {}
  }
  return res;
}

std::string find_executables(std::vector<std::string>& paths, std::string& command) {
  for (auto path : paths) {
    std::string full_path = path + "/" + command;
    if (access(full_path.c_str(), X_OK) == 0) {
      return full_path;
    }
    /*
    try {
      for (const auto& entry : std::filesystem::directory_iterator(path)) {
          When working with files, quotes are only needed in shell. Quotes are not used other than that, i.e., in file system. 
          This is because input in the shell are seperated by space, so we need a way to escape a space when necessary, which by 
          using quoting. 

          According to the C++ documentation (https://en.cppreference.com/w/cpp/filesystem/path.html), the return value for 
          the path class below, entry.path(), is a string. It will convert the file path into a string. 
          For example, you have a file at the location ./Documents/Hello World.exe, then it will return to you 
          "./Documents/Hello World.exe", without any quoting like the one in the shell. 

          The "command" variable that you pass to this function is the parsed (by the split_inputs function) version of 
          the input from the shell. It is already a normal path that our file system used to store things. Therefore, it is 
          unnecessary to put extra quoting to "command" since they are already aligned. 

          -------------------------
          From shell to file system
          -------------------------
          Quoted inputs (./"Hello' World.exe")
            -> go through split_inputs function
            -> Normal path (./Hello' World.exe) 
          This normal path is what our operating system used, and what C++ .path() class offer. Therefore, we can compare them. 

          The same thing applies when we want to send back command to the shell. We need to parse it such that spaces that are 
          treated as a character (not for splitting arguments) indeed treated as a character. We need to quote them when necessary.
        
        if (entry.path() == full_path) {
          return full_path;
        }
      }
    } catch (const std::filesystem::filesystem_error& e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
    */
  }
  return "";
}

/*
  The code below for enabling the raw mode is referenced from 
  https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html 
  I only did until Step 8 because the others are irrelevant for this project. 
*/

struct termios orig_termios;

void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);
  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
/*
  // This code is from Step 8 in https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html, used for testing only
  enableRawMode();
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
    if (iscntrl(c)) {
      printf("%d\n", c);
    } else {
      printf("%d ('%c')\n", c, c);
    }
  }
  return 0;
*/
  
  enableRawMode();

  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  const char* path = std::getenv("PATH"); // may return nullptr if it doesn't exist

  if (!path) {
    std::cout << "PATH is not set" << std::endl;
    return 1;
  }

  std::vector<std::string> paths = split_paths(path, ':');

  std::vector<std::string> hist;

  while (true) {
    std::cout << "$ ";

    std::string input = "";

    // Read character by character
    bool multiple_match_flag = false;
    while (true) {
      char c;
      read(STDIN_FILENO, &c, 1);
      if (c != '\t') {
        multiple_match_flag = false;
      }

      if (c == '\n') {
        std::cout << std::endl;
        break;
      } else if (c == 127) { // handling backspace, 127 refers to DEL
        if (input.empty()) continue;
        input.pop_back();
        std::cout << "\b \b";
        /*
          \b	Backspace: moves the cursor one character to the left
          ' '	Space: overwrites the character at the cursor with a space
          \b	Backspace again: moves the cursor back one more step to the left, so the cursor is in the correct position
        */
      } else if (c == '\t') {
        // Autocompletion should trigger here
        // std::cout << "Tab is triggered" << std::endl;
        std::set<std::string> all_commands = generate_executables(paths);
        for (const auto& builtin : {"exit", "type", "echo", "pwd"}) {
          all_commands.insert(builtin);
        }
        if (input.find(' ') != std::string::npos) continue;
        int cnt = 0;
        std::vector<std::string> matched;
        for (const auto& command : all_commands) {
          if (command.find(input) == 0) {
            cnt++;
            matched.push_back(command);
          }
        }
        if (cnt == 0) {
          std::cout << '\x07';
          continue;
        }
        if (cnt > 1) {
          int idx = input.size();
          bool exist_similar_prefix = false;
          while (true) {
            bool same_char = true;
            for (int i = 1; i < (int) matched.size(); i++) {
              if (idx >= matched[i].length()) {
                same_char = false;
                break;
              }
              if (matched[i][idx] != matched[i - 1][idx]) {
                same_char = false;
                break;
              }
            }
            if (same_char) {
              std::cout << matched[0][idx];
              input += matched[0][idx];
              exist_similar_prefix = true;
              idx++;
            } else {
              break;
            }
          }
          if (exist_similar_prefix) continue;
          if (!multiple_match_flag) {
            multiple_match_flag = true;
            std::cout << '\x07';
          } else {
            std::cout << std::endl;
            for (int i = 0; i < (int) matched.size(); i++) {
              if (i == (int) matched.size() - 1) {
                std::cout << matched[i] << std::endl;
              } else {
                std::cout << matched[i] << "  ";
              }
            }
            multiple_match_flag = false;
            std::cout << "$ " << input;
          }
          continue;
        }
        auto idx = matched[0].find(input) + input.size();
        while (idx < (int) matched[0].size()) {
          std::cout << matched[0][idx];
          input += matched[0][idx];
          idx++;
        }
        std::cout << " ";
        input += " ";
      } else {
        std::cout << c;
        input += c;
      }
    }
    
    hist.push_back(input);

    std::vector<std::string> args = split_inputs(input);

    if (args[0] == "exit") {
      return 0;
    } 
    
    if (args[0] == "type") {
      std::string command = args[1];
      if (command == "exit" || command == "type" || command == "echo" || command == "pwd" || command == "history") {
        std::cout << command << " is a shell builtin" << std::endl;
      } else if (std::string executables_path = find_executables(paths, command); executables_path != "") {
        std::cout << command << " is " << executables_path << std::endl;
      } else {
        std::cout << command << ": not found" << std::endl;
      }
      continue;
    } 
    
    if (args[0] == "echo") {
      for (int i = 1; i < (int) args.size(); i++) {
        std::cout << args[i];
        if (i == (int) args.size() - 1) std::cout << std::endl;
        else std::cout << " ";
      }
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

      // Case 3: Home directory
      if (args[1][0] == '~') {
        const char* home_dir = std::getenv("HOME");
        std::filesystem::current_path(home_dir);
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

    if (args[0] == "history") {
      if (args.size() > 1) {
        int num = std::stoi(args[1]);
        for (int i = (int) hist.size() - num; i < (int) hist.size(); i++) {
          std::cout << "    " << i + 1 << "  " << hist[i] << std::endl;
        }
        continue;
      }
      for (int i = 0; i < (int) hist.size(); i++) {
        std::cout << "    " << i + 1 << "  " << hist[i] << std::endl;
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
