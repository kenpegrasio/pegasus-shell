#include <filesystem>
#include <iostream>
#include <set>
#include <vector>

std::set<std::string> generate_executables(std::vector<std::string> &);

std::vector<std::string> list_matching_prefix(std::set<std::string> &,
                                              std::string &);

std::string find_longest_matching_prefix(std::vector<std::string> &,
                                         std::string &);

void autocompletion(std::vector<std::string> &, std::string &, bool &);