#include <string>
#include <iostream>
#include <type_traits>

#include "../src/constexpr-string.h"


#define IN_PLACE_DEF() \
  struct InPlace { \
    constexpr int GetVal() { return 10; } \
  } in_place;

#define SOME_STRUCT(NAME, ARG) \
  struct NAME { \
    NAME(int ARG) {} \
    static constexpr int GetVel(int ARG) { return ARG; } \
  }


std::string ToString(int i) {
  switch (i) {
    case GetVal("string=1"):
      return "1";
    case 2:
      return "2";
    default:
      return "default";
  }
}

template <int I>
struct SomeTemplate {};

int main(int argc, char** argv) {
  static_assert(*FindChar("k=1", '=') != 0, "");
  static_assert(HasChar("k=1", '='), "");
  static_assert(!HasChar("k=1", 'a'), "");
  static_assert(*FindEnd("m=3") == 0, "");
  static_assert(MyAtoi("34") == 34, "");
  static_assert(GetVal("n=0") == 0, "");
  std::cout << ToString(1) << std::endl;

  enum EEE { kNext };
  static_assert(kNext == 0, "");

  IN_PLACE_DEF()
  static_assert(in_place.GetVal() == 10, "");

  SomeTemplate<0> st;
  static_assert(sizeof(st) == 1, "");



  //ENUM_MEMBER_TO_VAL(kOne=1, MyEnum, 2);
  //static_assert(kNext == 2, "");

  // static_assert(ENUM_MEMBER_TO_VAL(k=1) == 1, "");
  //std::cout << MAYBE_ASSIGNMENT_TO_INT(k=666) << std::endl;
}

