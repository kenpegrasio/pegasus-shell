#include "echo_redirection_handler.h"

std::string find_output(std::vector<std::string>& args, int redirect_idx) {
  std::string res = "";
  for (int i = 1; i < redirect_idx; i++) {
    res += args[i];
    if (i == redirect_idx - 1)
      res += "\n";
    else
      res += " ";
  }
  return res;
}

void redirect_echo_stdout(std::vector<std::string>& args, int redirect_idx) {
  std::string res = find_output(args, redirect_idx);
  if (redirect_idx + 1 >= args.size()) {
    std::cerr << "No argument for redirect found" << std::endl;
    return;
  }
  std::ofstream outputFile(args[redirect_idx + 1]);
  outputFile << res;
}

void redirect_echo_stderr(std::vector<std::string>& args, int redirect_idx) {
  std::string res = find_output(args, redirect_idx);
  if (redirect_idx + 1 >= args.size()) {
    std::cerr << "No argument for redirect found" << std::endl;
    return;
  }
  std::ofstream outputFile(args[redirect_idx + 1]);
  std::cout << res;
  outputFile << "";
}

void append_echo_stdout(std::vector<std::string>& args, int redirect_idx) {
  std::string res = find_output(args, redirect_idx);
  if (redirect_idx + 1 >= args.size()) {
    std::cerr << "No argument for redirect found" << std::endl;
    return;
  }
  std::ofstream outputFile(args[redirect_idx + 1], std::ios::app);
  outputFile << res;
}

void append_echo_stderr(std::vector<std::string>& args, int redirect_idx) {
  std::string res = find_output(args, redirect_idx);
  if (redirect_idx + 1 >= args.size()) {
    std::cerr << "No argument for redirect found" << std::endl;
    return;
  }
  std::ofstream outputFile(args[redirect_idx + 1], std::ios::app);
  std::cout << res;
  outputFile << "";
}