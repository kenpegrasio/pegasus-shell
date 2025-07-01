#include <string>
#include <vector>
#include <iostream>
#include <fstream>

std::string find_output(std::vector<std::string>&, int);
void redirect_echo_stdout(std::vector<std::string>&, int);
void redirect_echo_stderr(std::vector<std::string>&, int);
void append_echo_stdout(std::vector<std::string>&, int);
void append_echo_stderr(std::vector<std::string>&, int);