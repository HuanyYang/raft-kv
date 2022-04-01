#ifndef RAFT_KV_LEPTDB_H
#define RAFT_KV_LEPTDB_H

#include "SkipList.h"
#include "string"
#include "wal.h"

class Leptdb {
public:
  explicit Leptdb(const std::string &dbname);

  Leptdb(const Leptdb &) = delete;
  Leptdb &operator=(const Leptdb &) = delete;

  ~Leptdb();

  bool Put(const std::string &key, const std::string &value);
  bool Delete(const std::string &key);
  bool Get(const std::string &key, std::string value);

  void CheckCompaction();

  void ShowWtable();
  void ShowRtable();

private:
  bool CreateDir();
  bool RecoverLogFile();
  bool MakeRoomForWrite();

  const std::string dbname_;

  memtable *wtable_;
  memtable *rtable_;
  WAL *wtableLog_;
  WAL *rtableLog_;
};

#endif // RAFT_KV_Leptdb_H
