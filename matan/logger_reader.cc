#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <chrono>
#include "Logger.hh"
#include "Reader.hh"

using namespace std::chrono;

void logit(const char* fname, const std::vector<std::string>& lyric_vec) {
  std::cout << fname << std::endl;
  matan::Logger bunchLogger(fname);
  auto start2 = high_resolution_clock::now();
  for (auto& lyric : lyric_vec) {
    bunchLogger << lyric << std::endl;
  }
  std::cout
    << duration_cast<nanoseconds>(high_resolution_clock::now()-start2).count()
    << std::endl;
}

void readit(const char* fname) {
  matan::Reader bunchReader(fname);
  while (!bunchReader.eof()) {
    auto line = bunchReader.getline();
    if (line.isValid()) {
      std::cout << *line.get() << std::endl;
    }
  }
}

int main(int argc, char** argv) {
  std::string lyrics = "Row, row, row your boat Gently down the stream, Merrily merrily, merrily, merrily Life is but a dream";
  std::istringstream iss(lyrics);
  std::vector<std::string> lyric_vec(std::istream_iterator<std::string>{iss},
                                     std::istream_iterator<std::string>{});
  std::cout << argv[1] << std::endl;
  std::ofstream myOstream(argv[1], std::ios::out);
  std::ifstream myIstream(argv[1], std::ios::in);
  std::string line;
  auto start1 = high_resolution_clock::now();
  for (auto& lyric : lyric_vec) {
    myOstream << lyric << std::endl;
    myOstream.flush();
  }
  while (std::getline(myIstream, line)) {
    std::cout << line << std::endl;
  }
  std::cout
    << duration_cast<nanoseconds>(high_resolution_clock::now()-start1).count()
    << std::endl;
  myIstream.close();
  myOstream.close();
  
  logit(argv[2], lyric_vec);
  std::cout << "    finish logger" << std::endl;
  readit(argv[2]);
  std::cout << "    finish reader" << std::endl;
  
  return 0;
}
