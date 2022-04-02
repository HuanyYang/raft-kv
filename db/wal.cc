#include "wal.h"
#include <unistd.h>

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

size_t WAL::LoadLogToMem(SkipList<std::string, std::string> *mem) {
  logReader_.open(logPath_);
  std::string line, key, value;
  size_t size = 0;
  while (getline(logReader_, line)) {
    splitKV(line, key, value);
    mem->insertElement(key, value);
    size += key.size();
    size += value.size();
  }
  logReader_.close();
  return size;
}

bool LoadCurLogToOldLog(WAL *cur, WAL *old) {
  std::cout << "LoadCurLogToOldLog" << std::endl;
  cur->logReader_.open(cur->logPath_);
  old->logWriter_.open(old->logPath_, std::ios_base::app);
  std::string line;
  while (getline(cur->logReader_, line)) {
    old->logWriter_ << line << "\n";
  }
  // 清空curLog
  cur->logReader_.close();
  cur->logWriter_.open(cur->logPath_);
  cur->logWriter_.close();
  old->logWriter_.flush();
  old->logWriter_.close();
  return true;
}

void splitKV(const std::string &str, std::string &key, std::string &value) {
  if (str.empty() || str.find(delimiter) == std::string::npos) {
    return;
  }
  key = str.substr(0, str.find(delimiter));
  value = str.substr(str.find(delimiter) + 1, str.size());
}
