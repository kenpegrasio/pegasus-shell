#include <fstream>
#include <iostream>
#include <vector>

void read_history_from_file(std::vector<std::string> &, std::string);
void write_history_to_file(std::vector<std::string> &, std::string);
void append_history_to_file(std::vector<std::string> &, std::string);
void display_history(std::vector<std::string> &, int);