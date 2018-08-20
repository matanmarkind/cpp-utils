#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <fstream>

// Example of what we are looking for.

/*
#define _NUMARGS(X5, X4, X3, X2, X1, N, ...)   N
#define NUMARGS(...) _NUMARGS(__VA_ARGS__, 5, 4, 3, 2, 1)

#define EXPAND(X)       X
#define FIRSTARG(X, ...)    (X)
#define RESTARGS(X, ...)    (__VA_ARGS__)
#define FOREACH(MACRO, LIST)    FOREACH_(NUMARGS LIST, MACRO, LIST)
#define FOREACH_(N, M, LIST)    FOREACH__(N, M, LIST)
#define FOREACH__(N, M, LIST)   FOREACH_##N(M, LIST)
#define FOREACH_1(M, LIST)  M LIST

#define FOREACH_2(M, LIST)  EXPAND(M FIRSTARG LIST) FOREACH_1(M, RESTARGS LIST)
#define FOREACH_3(M, LIST)  EXPAND(M FIRSTARG LIST) FOREACH_2(M, RESTARGS LIST)
#define FOREACH_4(M, LIST)  EXPAND(M FIRSTARG LIST) FOREACH_3(M, RESTARGS LIST)
#define FOREACH_5(M, LIST)  EXPAND(M FIRSTARG LIST) FOREACH_4(M, RESTARGS LIST)
*/

std::string Make_NUMARGS(int max_num_args) {
  std::stringstream _numargs;
  _numargs << "#define _NUMARGS(";
  std::stringstream numargs;
  numargs << "#define NUMARGS(...) _NUMARGS(__VA_ARGS__, ";
  for (int i = max_num_args; i > 1; --i) {
    _numargs << "X" << i << ", ";
    numargs << i << ", ";
  }
  numargs << "1)";

  _numargs << "X1, N, ...) N" << "\n\n" << numargs.str();
  return _numargs.str();
}

// Macros that don't depend on on NUM desired.
std::string MakeBaseMacros() {
  return "#define EXPAND(X)    X\n"
         "#define FIRSTARG(X, ...)    (X)\n"
         "#define RESTARGS(X, ...)    (__VA_ARGS__)\n"
         "#define FOREACH(MACRO, LIST)    FOREACH_(NUMARGS LIST, MACRO, LIST)\n"
         "#define FOREACH_(N, M, LIST)    FOREACH__(N, M, LIST)\n"
         "#define FOREACH__(N, M, LIST)   FOREACH_##N(M, LIST)\n"
         "#define FOREACH_1(M, LIST)  M LIST";
}

// Make all the FOREACH_X lines required.
std::string MakeForEaches(int max_num_args) {
  std::stringstream for_eaches;
  for (int i = 2; i <= max_num_args; ++i) {
    for_eaches << "#define FOREACH_"
               << i
               << "(M, LIST)  EXPAND(M FIRSTARG LIST) FOREACH_"
               << (i-1)
               << "(M, RESTARGS LIST)\n";
  }
  return for_eaches.str();
}

int main(int argc, char** argv) {
  assert(argc == 3);
  int max_num_args = std::atoi(argv[1]);

  // Probably shouldn't change the file name since other files will use that
  // for imports.
  std::ofstream outfile;
  outfile.open(argv[2]);

  // TODO: probably want some #ifdef action.
  outfile << Make_NUMARGS(max_num_args) << "\n\n";
  outfile << MakeBaseMacros() << "\n\n";
  outfile << MakeForEaches(max_num_args) << "\n\n";

  return 0;
}
