#ifndef RAFT_KV_LEPTDB_H
#define RAFT_KV_LEPTDB_H

#include "manifest.h"
#include "skiplist.h"
#include "string"
#include "wal.h"

const size_t MEMTableSizeLimit = 512;

class Leptdb {
public:
  explicit Leptdb(const std::string &dbname);

  Leptdb(const Leptdb &) = delete;
  Leptdb &operator=(const Leptdb &) = delete;

  ~Leptdb();

  bool Put(const std::string &key, const std::string &value);
  bool Delete(const std::string &key);
  bool Get(const std::string &key, std::string &value);

  void ShowWtable();
  void ShowRtable();
  void ShowManifest();
  size_t getMemtableSize();

private:
  bool CreateDir();
  bool RecoverLogFile();
  void MakeRoomForWrite();
  bool Flush();
  void FlushRTable();
  void BuildSST(int level, int seq);
  bool SearchSST(const std::string &key, std::string &value);
  void CompactSST();

  const std::string dbname_;

  memtable *wtable_;
  memtable *rtable_;
  WAL *wtableLog_;
  WAL *rtableLog_;

  size_t memtableSize_;

  Manifest *manifest;
};

#endif // RAFT_KV_LEPTDB_H
