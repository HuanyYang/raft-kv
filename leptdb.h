#ifndef RAFT_KV_LEPTDB_H
#define RAFT_KV_LEPTDB_H

#include "SkipList.h"
#include "string"
#include "wal.h"

class Leptdb {
public:
  Leptdb(const std::string &dbname);

  Leptdb(const Leptdb &) = delete;
  Leptdb &operator=(const Leptdb &) = delete;

  ~Leptdb();

  bool Put(const std::string &key, const std::string &value);
  bool Delete(const std::string &key);
  bool Get(const std::string &key, std::string value);

private:
  bool RecoverLogFile();
  bool MakeRoomForWrite();

  const std::string dbname_;

  SkipList<std::string, std::string> *wtable_;
  SkipList<std::string, std::string> *rtable_;
  WAL *logfile_;
};

#endif // RAFT_KV_Leptdb_H
