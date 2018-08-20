#include <cmath>
#include <climits>

// Find the first occurance of c in str. If unfound returns
// a pointer to the terminating NULL.
// TODO: consider returning nullptr instead if not found.
constexpr const char* FindChar(const char* str, char c) {
  return str[0] == c ?
          str :
          (str[0] == 0 ? str : FindChar(str+1, c));
}

// Returns true if found a non NULL char c before reaching
// the terminating NULL.
constexpr bool HasChar(const char* str, char c) {
  return *FindChar(str, c) != 0;
}

// Return pointer to  NULL terminator at compile time.
constexpr const char* FindEnd(const char* str) {
  return str[0] == 0 ? str : FindEnd(str + 1);
}

// Convert a base 10 string integer to an int.
constexpr int MyAtoiImpl(const char* str, const char* end) {
  return str == end ?
          0 :
          (str[0] - '0') * std::pow(10, end - str - 1) + MyAtoiImpl(str + 1, end);
}

// Convert a base 10 string to int. Assumes the number goes
// from str until the null terminator.
constexpr int MyAtoi(const char* str) {
  return MyAtoiImpl(str, FindEnd(str));
}

constexpr int GetVal(const char* str) {
  return MyAtoi(FindChar(str, '=') + 1);
}

// Takes either an Enum named val (kVal) or an integer assignment (kVal=7) and
// places the integer value where it should go.
// CANNOT EXPLICITLY ASSIGN TO BE MAXINT!!!!!!!!!!!!!!!!!!!!!!!!!!
#define ENUM_MEMBER_TO_VAL(CASE, NAME, N)                                 \
  {enum class __RESERVED_ENUM_ ##NAME ## _ ## N { CASE, kNext };, 1}
