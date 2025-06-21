#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Uncomment this block to pass the first stage
  while (true) {
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);
    if (input == "exit 0") {
      return 0;
    }
    if (input.substr(0, 4) == "echo") {
      std::cout << input.substr(5, (int) input.size() - 4) << std::endl;
      continue;
    }
    std::cout << input << ": command not found" << std::endl;
  }
}
