#include "autocompletion_handler.h"

std::set<std::string> generate_executables(std::vector<std::string> &paths) {
  std::set<std::string> res;
  for (const auto &path : paths) {
    try {
      for (const auto &entry : std::filesystem::directory_iterator(path)) {
        res.insert(entry.path().filename().string());
      }
    } catch (const std::filesystem::filesystem_error &e) {
    }
  }
  return res;
}

std::vector<std::string> list_matching_prefix(
    std::set<std::string> &all_commands, std::string &input) {
  std::vector<std::string> matched;
  for (const auto &command : all_commands) {
    if (command.find(input) == 0) {
      matched.push_back(command);
    }
  }
  return matched;
}

std::string find_longest_matching_prefix(std::vector<std::string> &matched,
                                 std::string &input) {
  std::string res = "";
  int idx = input.size();
  while (true) {
    bool same_char = true;
    for (int i = 1; i < (int)matched.size(); i++) {
      if (idx >= matched[i].length() ||
          matched[i][idx] != matched[i - 1][idx]) {
        same_char = false;
        break;
      }
    }
    if (same_char) {
      res += matched[0][idx];
      idx++;
      continue;
    }
    break;
  }
  return res;
}

void autocompletion(std::vector<std::string> &paths, std::string &input,
                    bool &multiple_match_flag) {
  std::set<std::string> all_commands = generate_executables(paths);
  for (const auto &builtin : {"exit", "type", "echo", "pwd", "history"}) {
    all_commands.insert(builtin);
  }
  if (input.find(' ') != std::string::npos) return;

  auto matched = list_matching_prefix(all_commands, input);
  int cnt = matched.size();

  if (cnt == 0) {
    std::cout << '\x07';
    return;
  }

  if (cnt > 1) {
    int idx = input.size();
    std::string matching_prefix = find_longest_matching_prefix(matched, input);
    if (matching_prefix != "") {
        input += matching_prefix;
        std::cout << matching_prefix;
        return;
    }
    if (!multiple_match_flag) {
      multiple_match_flag = true;
      std::cout << '\x07';
    } else {
      std::cout << std::endl;
      for (int i = 0; i < (int)matched.size(); i++) {
        if (i == (int)matched.size() - 1) {
          std::cout << matched[i] << std::endl;
        } else {
          std::cout << matched[i] << "  ";
        }
      }
      multiple_match_flag = false;
      std::cout << "$ " << input;
    }
    return;
  }
  auto idx = matched[0].find(input) + input.size();
  while (idx < (int)matched[0].size()) {
    std::cout << matched[0][idx];
    input += matched[0][idx];
    idx++;
  }
  std::cout << " ";
  input += " ";
}