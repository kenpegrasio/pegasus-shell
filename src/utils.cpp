#include "utils.h"

std::vector<std::string> split_shell_inputs(std::string& input) {
  std::string cur = "";
  std::vector<std::string> v;
  bool inside_single_quote = false;
  bool inside_double_quote = false;
  for (int i = 0; i < (int)input.size(); i++) {
    if (!inside_single_quote && input[i] == '\"') {
      inside_double_quote = !inside_double_quote;
      continue;
    }
    if (!inside_double_quote && input[i] == '\'') {
      inside_single_quote = !inside_single_quote;
      continue;
    }
    if (input[i] == '\\') {
      if (inside_single_quote) {
        /*
          Enclosing characters in single quotes (‘'’) preserves the literal
          value of each character within the quotes. A single quote may not
          occur between single quotes, even when preceded by a backslash.
          Source:
          https://www.gnu.org/software/bash/manual/bash.html#Single-Quotes
        */
        cur += input[i];
      } else if (inside_double_quote) {
        /*
          The backslash retains its special meaning only when followed by one of
          the following characters: ‘$’, ‘`’, ‘"’, ‘\’, or newline. Within
          double quotes, backslashes that are followed by one of these
          characters are removed. Backslashes preceding characters without a
          special meaning are left unmodified. Source:
          https://www.gnu.org/software/bash/manual/bash.html#Double-Quotes Note:
          In C++ '\n' is treated as a single character
        */
        bool isSpecialCharacter =
            (input[i + 1] == '\n' || input[i + 1] == '$' ||
             input[i + 1] == '\"' || input[i + 1] == '`' ||
             input[i + 1] == '\\');

        if (i + 1 < input.length() && isSpecialCharacter) {
          cur += input[i + 1];
          i++;
        } else {
          cur += input[i];
        }
      } else {
        /*
          A non-quoted backslash ‘\’ is the Bash escape character. It preserves
          the literal value of the next character that follows. Source:
          https://www.gnu.org/software/bash/manual/bash.html#Escape-Character
        */
        if (i + 1 < input.length()) {
          cur += input[i + 1];
          i++;
        } else {
          throw std::invalid_argument("\\ must be followed by a character");
        }
      }
      continue;
    }

    if (!inside_double_quote && !inside_single_quote && input[i] == ' ') {
      if (cur == "") continue;
      v.push_back(cur);
      cur = "";
    } else {
      cur += input[i];
    }
  }
  v.push_back(cur);
  return v;
}

std::vector<std::string> split_env_paths(const char*& path, char delimiter) {
  std::string cur = "";
  std::vector<std::string> v;
  for (int i = 0; i < strlen(path); i++) {
    if (path[i] == delimiter) {
      v.push_back(cur);
      cur = "";
    } else {
      cur += path[i];
    }
  }
  v.push_back(cur);
  return v;
}

std::vector<std::string> split_directory_folders(std::string path,
                                                 char delimiter) {
  std::string cur = "";
  std::vector<std::string> v;
  for (int i = 0; i < (int)path.size(); i++) {
    if (path[i] == delimiter) {
      v.push_back(cur);
      cur = "";
    } else {
      cur += path[i];
    }
  }
  v.push_back(cur);
  return v;
}

/*
    --------------------------
    QUOTING IN SHELL VS OTHERS
    --------------------------

    When working with files, quotes are only needed in shell. Quotes are not
   used other than that, i.e., in file system. This is because input in the
   shell are seperated by space, so we need a way to escape a space when
   necessary, which by using quoting.

    According to the C++ documentation
   (https://en.cppreference.com/w/cpp/filesystem/path.html), the return value
   for the path class below, entry.path(), is a string. It will convert the file
   path into a string. For example, you have a file at the location
   ./Documents/Hello World.exe, then it will return to you
    "./Documents/Hello World.exe", without any quoting like the one in the
   shell.

    The "command" variable that you pass to this function is the parsed (by the
   split_shell_inputs function) version of the input from the shell. It is
   already a normal path that our file system used to store things. Therefore,
   it is unnecessary to put extra quoting to "command" since they are already
   aligned.


    From shell to file system
    -------------------------
    Quoted inputs (./"Hello' World.exe")
    -> go through split_inputs function
    -> Normal path (./Hello' World.exe)
    This normal path is what our operating system used, and what C++ .path()
   class offer. Therefore, we can compare them.

    The same thing applies when we want to send back command to the shell. We
   need to parse it such that spaces that are treated as a character (not for
   splitting arguments) indeed treated as a character. We need to quote them
   when necessary.
*/

std::string find_entries_from_paths(std::vector<std::string>& paths,
                                    std::string& command) {
  for (auto path : paths) {
    std::string full_path = path + "/" + command;
    try {
      for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path() == full_path) {
          // This if statement just check whether the full_path exists or not,
          // this does not check whether full_path is executable or not To check
          // whether the function is executables, use the
          // find_executables_from_paths function
          return full_path;
        }
      }
    } catch (const std::filesystem::filesystem_error& e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
  }
  return "";
}

std::string find_executables_from_paths(std::vector<std::string>& paths,
                                        std::string& command) {
  for (auto path : paths) {
    std::string full_path = path + "/" + command;
    if (access(full_path.c_str(), X_OK) == 0) {
      return full_path;
    }
  }
  return "";
}

std::vector<std::vector<std::string>> split_arguments_for_pipeline(
    std::vector<std::string>& args, std::string delimiter) {
  std::vector<std::vector<std::string>> res;
  std::vector<std::string> cur;
  for (int i = 0; i < (int)args.size(); i++) {
    if (args[i] == delimiter) {
      res.push_back(cur);
      cur.clear();
      continue;
    }
    cur.push_back(args[i]);
  }
  return res;
}