#pragma once

#include "AsyncWorker.hh"
#include <fstream>
#include <string>

namespace matan {

class Reader final : public AsyncWorker {
  /*
   * Reads lines from a file and stores them in a BunchQueue.
   * Should allow for multiple readers to write to a single BunchQueue
   */
public:
  Reader(const std::string& ofname, int numLinesToBuffer=4); //arbitrary choice
  virtual ~Reader();
  Reader(const Reader&) = delete;
  const Option<const std::string*> getline();
  bool isEmpty();
  bool eof() const { return m_ifstream.eof(); }

private:
  void doFlush();
  virtual void doit() override;
  virtual bool shouldSleep() override;

  
  std::ifstream m_ifstream;
  BunchQueue<std::string> m_contents;
  const int m_numLinesToBuffer;
  const VecQueue<std::string>* m_pBufferedLines;
  int m_iLine = 0;
};

} // matan