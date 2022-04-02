#ifndef RAFT_KV_LEPTDB_H
#define RAFT_KV_LEPTDB_H

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
  bool Get(const std::string &key, std::string value);

  void ShowWtable();
  void ShowRtable();

private:
  bool CreateDir();
  bool RecoverLogFile();
  bool Flush();
  void MakeRoomForWrite();
  void FlushRTable();
  void CompactSST();

  const std::string dbname_;

  memtable *wtable_;
  memtable *rtable_;
  WAL *wtableLog_;
  WAL *rtableLog_;

  size_t memtableSize_;
};

#endif // RAFT_KV_LEPTDB_H
