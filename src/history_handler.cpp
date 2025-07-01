#include "history_handler.h"

void read_history_from_file(std::vector<std::string> &hist,
                            std::string path_to_file) {
  std::ifstream inputFile(path_to_file);
  if (!inputFile.is_open()) {
    std::cerr << "Error opening the input file" << std::endl;
    return;
  }
  std::string line;
  while (std::getline(inputFile, line)) {
    hist.push_back(line);
  }
  inputFile.close();
}

void write_history_to_file(std::vector<std::string> &hist,
                           std::string path_to_file) {
  std::ofstream outFile(path_to_file);
  for (auto history_entry : hist) {
    outFile << history_entry << std::endl;
  }
  outFile.close();
}

void append_history_to_file(std::vector<std::string> &hist,
                            std::string path_to_file) {
  std::ofstream outFile(path_to_file, std::ios::app);
  for (auto history_entry : hist) {
    outFile << history_entry << std::endl;
  }
  hist.clear();
  outFile.close();
}

void display_history(std::vector<std::string> &hist, int start) {
  for (int i = start; i < (int)hist.size(); i++) {
    std::cout << "    " << std::to_string(i + 1) << "  " << hist[i]
              << std::endl;
  }
}