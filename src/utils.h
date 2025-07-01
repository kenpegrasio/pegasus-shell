#include <vector>
#include <string>
#include <stdexcept>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <unistd.h>

std::vector<std::string> split_shell_inputs(std::string&);
std::vector<std::string> split_env_paths(const char*&, char);
std::vector<std::string> split_directory_folders(std::string, char);
std::string find_entries_from_paths(std::vector<std::string>&, std::string&);
std::string find_executables_from_paths(std::vector<std::string>&,
                                        std::string&);