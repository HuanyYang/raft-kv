#include "leptdb.h"
#include <sys/stat.h>

Leptdb::Leptdb(const std::string &dbname)
    : dbname_(dbname), wtable_(new memtable), rtable_(new memtable),
      wtableLog_(new WAL(dbname + "/cur.log")),
      rtableLog_(new WAL(dbname + "/old.log")) {
  this->CreateDir();
  this->RecoverLogFile();
}

Leptdb::~Leptdb() {
  if (wtable_ != nullptr)
    delete wtable_;
  if (rtable_ != nullptr)
    delete rtable_;
  if (wtableLog_ != nullptr)
    delete wtableLog_;
  if (rtableLog_ != nullptr)
    delete rtableLog_;
}

bool Leptdb::Put(const std::string &key, const std::string &value) {
  if (wtableLog_->AddRecord(key, value)) {
    return wtable_->insertElement(key, value);
  }
  return false;
}

bool Leptdb::Delete(const std::string &key) { return Put(key, ""); }

bool Leptdb::Get(const std::string &key, std::string value) {
  if (wtable_->searchElement(key, value) ||
      rtable_->searchElement(key, value)) {
    if (!value.empty())
      return true;
  }
  // TODO, search sst
  return false;
}

void Leptdb::CheckCompaction() { std::cout << "CheckCompaction" << std::endl; }

void Leptdb::ShowWtable() { wtable_->displayList(); }
void Leptdb::ShowRtable() { rtable_->displayList(); }

bool Leptdb::CreateDir() {
  if (mkdir(dbname_.data(), S_IRWXU) != 0)
    return false;
  return true;
}

bool Leptdb::RecoverLogFile() {
  std::cout << "Recover: " << dbname_ << std::endl;
  wtableLog_->LoadLogToMem(wtable_);
  rtableLog_->LoadLogToMem(rtable_);
  CheckCompaction();
  return true;
}

bool Leptdb::MakeRoomForWrite() { return false; }
