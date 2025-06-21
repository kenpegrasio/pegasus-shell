#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

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
      } 
      else {
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
