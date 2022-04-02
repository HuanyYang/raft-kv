#include "leptdb.h"
#include <sys/stat.h>

Leptdb::Leptdb(const std::string &dbname)
    : dbname_(dbname), wtable_(new memtable), rtable_(new memtable),
      wtableLog_(new WAL(dbname + "/cur.log")),
      rtableLog_(new WAL(dbname + "/old.log")), memtableSize_(0) {
  this->CreateDir();
  this->RecoverLogFile();
  std::cout << "open DB: " << dbname << ", memtableSize: " << memtableSize_
            << std::endl;
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
  MakeRoomForWrite();
  if (wtableLog_->AddRecord(key, value) && wtable_->insertElement(key, value)) {
    memtableSize_ += key.size();
    memtableSize_ += value.size();
    return true;
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

void Leptdb::ShowWtable() {
  std::cout << "ShowWtable: ";
  wtable_->displayList();
}
void Leptdb::ShowRtable() {
  std::cout << "ShowRtable: ";
  rtable_->displayList();
}

bool Leptdb::CreateDir() {
  if (mkdir(dbname_.data(), S_IRWXU) != 0)
    return false;
  return true;
}

bool Leptdb::RecoverLogFile() {
  memtableSize_ += wtableLog_->LoadLogToMem(wtable_);
  rtableLog_->LoadLogToMem(rtable_);
  return true;
}

// 将rtable写入sst，清空skipList和wal
bool Leptdb::Flush() {
  std::cout << "Flush" << std::endl;
  return false;
}

void Leptdb::MakeRoomForWrite() {
  while (true) {
    if (memtableSize_ < MEMTableSizeLimit) {
      break;
    } else if (rtable_->elementCount() > 0) {
      // wtable满且rtable非空，等待flush
      // TODO flush rtable -> sst
      Flush();
    } else {
      // wtable dump为rtable，目前用替换实现
      // 替换跳表指针，wtableLog_输出到rtableLog_
      LoadCurLogToOldLog(wtableLog_, rtableLog_);
      std::swap(wtable_, rtable_);
      memtableSize_ = 0;
    }
  }
}

void Leptdb::CheckCompaction() { std::cout << "CheckCompaction" << std::endl; }
