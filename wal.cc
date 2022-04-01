#include "wal.h"
#include <unistd.h>

void splitKV(const std::string &str, std::string &key, std::string &value) {
  if (str.empty() || str.find(delimiter) == std::string::npos) {
    return;
  }
  key = str.substr(0, str.find(delimiter));
  value = str.substr(str.find(delimiter) + 1, str.size());
}

// WAL不存在则创建
WAL::WAL(const std::string &logPath) : logPath_(logPath) {
  if (access(logPath_.c_str(), F_OK) != 0) {
    logWriter_.open(logPath_);
  }
}

WAL::~WAL() {
  if (logWriter_.is_open())
    logWriter_.close();
  if (logReader_.is_open())
    logReader_.close();
}

bool WAL::AddRecord(const std::string &key, const std::string &value) {
  logWriter_.open(logPath_, std::ios_base::app);
  logWriter_ << key << delimiter << value << "\n";
  logWriter_.flush();
  logWriter_.close();
  return true;
}

bool WAL::LoadLogToMem(SkipList<std::string, std::string> *mem) {
  logReader_.open(logPath_);
  std::string line, key, value;
  while (getline(logReader_, line)) {
    splitKV(line, key, value);
    mem->insertElement(key, value);
  }
  logReader_.close();
  return true;
}
