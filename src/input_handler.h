#include <termios.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "autocompletion_handler.h"

extern struct termios orig_termios;

void enableRawMode();
void disableRawMode();
void read_input_per_char(std::string&, std::vector<std::string>&,
                         std::vector<std::string>&, int&);