#include "Logger.hh"


namespace matan {


/**********************    BunchLogger    *******************************/


Logger::Logger(const std::string& ofname) :
    AsyncWorker(),
    m_ofstream(ofname, std::ios::out) {
  assert(m_ofstream.is_open());
  init();
}

Logger::~Logger() {
  doFlush();
  done();
  m_ofstream.close();
}

void Logger::flush() {
  if (m_buf.size() > MAX_LEN) {
    doFlush();
  }
}

void Logger::doFlush() {
  m_contents.push_back(m_buf);
  notifyWorker();
  m_buf.clear();
}

void Logger::doit() {
  for (const auto& line : m_contents.takeQueue()) {
    m_ofstream << line;
    m_ofstream.flush();
  }
}

} // matan
