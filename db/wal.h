#ifndef RAFT_KV_WAL_H
#define RAFT_KV_WAL_H

#include "fstream"
#include "skiplist.h"

const char delimiter = ':';

class WAL {
public:
  explicit WAL(const std::string &logPath);

  WAL(const WAL &) = delete;
  WAL &operator=(const WAL &) = delete;

  ~WAL();

  bool AddRecord(const std::string &key, const std::string &value);

  // 将WAL中的内容恢复到memtable
  size_t LoadLogToMem(memtable *mem);

  void ClearLog();

private:
  std::string logPath_;
  std::ofstream logWriter_;
  std::ifstream logReader_;

  friend bool LoadCurLogToOldLog(WAL *cur, WAL *old);
};

bool LoadCurLogToOldLog(WAL *cur, WAL *old);

void splitKV(const std::string &str, std::string &key, std::string &value);

#endif // RAFT_KV_WAL_H
