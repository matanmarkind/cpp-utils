#include "Reader.hh"
#include <iostream>
#include <string>

namespace matan {

Reader::Reader(const std::string& ifname, int numLinesToBuffer) :
  m_ifstream(ifname, std::ios::in),
  m_contents(numLinesToBuffer),
  m_numLinesToBuffer(numLinesToBuffer),
  m_pBufferedLines(&m_contents.takeQueue()) {
    assert(m_ifstream.is_open());
    init();
}

Reader::~Reader() {
  done();
  m_ifstream.close();
}

bool Reader::isEmpty() {
  /*
   * Are there any buffered lines. Doesn't say anything about underlying file.
   * It seems a bit weird that in an is function I have this notify, but it
   * makes the most sense to notify when I take a new queue
   */ 
  if (m_iLine >= m_pBufferedLines->size()) {
    m_pBufferedLines = &m_contents.takeQueue();
    notifyWorker();
    m_iLine = 0;
  }
  return m_iLine >= m_pBufferedLines->size();
}

const Option<const std::string*> Reader::getline() {
  if (isEmpty()) {
    return {};
  }
  return {&((*m_pBufferedLines)[m_iLine++])};
}

void Reader::doit() {
  std::string line;
  while (m_contents.size() < m_numLinesToBuffer && std::getline(m_ifstream, line)) {
    m_contents.push_back(std::move(line));
    line.clear();
  }
}

bool Reader::shouldSleep() {
  if (m_contents.size() >= m_numLinesToBuffer) {
    return true;
  }
  m_ifstream.clear();
  return m_ifstream.peek() == std::char_traits<char>::eof();
}
} //namespace matan