#include "leptdb.h"
#include <sys/stat.h>

Leptdb::Leptdb(const std::string &dbname)
    : dbname_(dbname), wtable_(new memtable), rtable_(new memtable),
      wtableLog_(new WAL(dbname + "/cur.log")),
      rtableLog_(new WAL(dbname + "/old.log")), memtableSize_(0),
      manifest(new Manifest(dbname + "/manifest")) {
  this->CreateDir();
  this->manifest->RecoverManifest();
  this->RecoverLogFile();
  std::cout << "open DB: " << dbname << ", memtableSize: " << memtableSize_
            << std::endl;
  ShowManifest();
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
  }
  return true;
}

bool Leptdb::Delete(const std::string &key) { return Put(key, ""); }

bool Leptdb::Get(const std::string &key, std::string &value) {
  std::string tmpValue;
  if (wtable_->searchElement(key, tmpValue) ||
      rtable_->searchElement(key, tmpValue) || SearchSST(key, tmpValue)) {
    // value为空表示已被删除
    if (!tmpValue.empty()) {
      value = tmpValue;
      return true;
    }
  }
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

void Leptdb::ShowManifest() { manifest->ShowManifest(); }

size_t Leptdb::getMemtableSize() { return memtableSize_; }

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

void Leptdb::MakeRoomForWrite() {
  while (true) {
    if (memtableSize_ < MEMTableSizeLimit) {
      break;
    } else if (rtable_->elementCount() > 0) {
      // wtable满且rtable非空，flush rtable -> sst
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

bool Leptdb::Flush() {
  // TODO first check level-0 sst dump
  FlushRTable();
  return true;
}

void Leptdb::FlushRTable() {
  // 直接dump成level-0的sst，清空rtalbe_和rtableLog_
  int count = manifest->getLeveCount(0) + 1;
  BuildSST(0, count);
  manifest->setLevelCount(0, count);
  rtable_->Clear();
  rtableLog_->ClearLog();
}

std::string makeSeq(int seq) {
  if (seq < 10) {
    return "0" + std::to_string(seq);
  } else {
    return std::to_string(seq);
  }
}

void Leptdb::BuildSST(int level, int seq) {
  std::string sstPath = dbname_ + "/" + makeSeq(level) + makeSeq(seq) + ".sst";
  std::ofstream ofs(sstPath);

  auto iter = rtable_->Iter();
  while (iter) {
    ofs << iter->getKey() << delimiter << iter->getValue() << "\n";
    rtable_->ForwardIter(iter);
  }
  ofs.flush();
  ofs.close();
}

bool Leptdb::SearchSST(const std::string &key, std::string &value) {
  std::ifstream ifs;
  std::string sstPath, line, tmpK, tmpV;
  for (int level = 0; level < manifest->getTotalLevel(); ++level) {
    for (int seq = manifest->getLeveCount(level); seq > 0; --seq) {
      sstPath = dbname_ + "/" + makeSeq(level) + makeSeq(seq) + ".sst";
      //      std::cout << "sstPath: " << sstPath << std::endl;
      ifs.open(sstPath);
      while (getline(ifs, line)) {
        splitKV(line, tmpK, tmpV);
        if (tmpK == key && !tmpV.empty()) {
          value = tmpV;
          return true;
        }
      }
      ifs.close();
    }
  }
  return false;
}

void Leptdb::CompactSST() {}
