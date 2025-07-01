#include "input_handler.h"

/*
  The code below for enabling the raw mode is referenced from
  https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
  I only did until Step 8 because the others are irrelevant for this project.
*/

struct termios orig_termios;

void disableRawMode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); }

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);
  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void read_input_per_char(std::string& input, std::vector<std::string>& paths,
                         std::vector<std::string>& hist, int& hist_idx) {
  bool multiple_match_flag = false;
  while (true) {
    char c;
    read(STDIN_FILENO, &c, 1);
    if (c != '\t') multiple_match_flag = false;

    if (c == '\n') {
      std::cout << std::endl;
      break;
    }

    if (c == 127) {  // handling backspace, 127 refers to DEL
      if (input.empty()) continue;
      input.pop_back();
      std::cout << "\b \b";
      continue;
    }

    if (c == '\t') {
      autocompletion(paths, input, multiple_match_flag);
      continue;
    }

    if (c == '\x1B') {
      char seq[2];
      if (read(STDIN_FILENO, &seq[0], 1) == 0) continue;
      if (read(STDIN_FILENO, &seq[1], 1) == 0) continue;
      if (seq[0] == '[' && seq[1] == 'A') {  // up arrow handler
        input = "";
        if (hist_idx > 0) hist_idx--;
        std::cout << "\33[2K\r";
        std::cout << "$ ";
        std::cout << hist[hist_idx];
        input = hist[hist_idx];
      }
      if (seq[0] == '[' && seq[1] == 'B') {  // down arrow handler
        input = "";
        if (hist_idx + 1 < (int)hist.size()) hist_idx++;
        std::cout << "\33[2K\r";
        std::cout << "$ ";
        std::cout << hist[hist_idx];
        input = hist[hist_idx];
      }
      continue;
    }

    std::cout << c;
    input += c;
  }
}