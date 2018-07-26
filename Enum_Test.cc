#include <string>
#include <iostream>

#include "Enum.h"

ENUM(PrintableEnum, int, kMin, I, can, have, however, many, values, you,
     desire, but, none, may, be, user, specified, kMax);

int main(int argc, char** argv) {
  PrintableEnum pe(PrintableEnum::I);
  std::cout << pe.ToString() << " " << pe.value() << std::endl;  
  
  pe.val_ = PrintableEnum::kMax;
  std::cout << pe.ToString() << " " << pe.value() << std::endl;
    
  std::cout << STRINGIFY(ENUM(PE, int, kMin, kMax)) << std::endl;
  
  return 0;
}
