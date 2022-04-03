#include "manifest.h"
#include "wal.h"

Manifest::Manifest(const std::string &filePath) : filePath_(filePath) {}

Manifest::~Manifest() {
  if (manifestWriter_.is_open())
    manifestWriter_.close();
  if (manifestReader_.is_open())
    manifestReader_.close();
}

void Manifest::RecoverManifest() {
  if (access(filePath_.c_str(), F_OK) != 0) {
    manifestWriter_.open(filePath_);
  }
  manifestWriter_.close();
  RecoverLevelInfo();
}

void Manifest::ShowManifest() {
  std::cout << "ShowManifest:" << std::endl;
  for (int i = 0; i < levelCounter_.size(); ++i) {
    std::cout << "Level-" << i << " sst counts: " << levelCounter_[i]
              << std::endl;
  }
}

int Manifest::getLeveCount(int level) { return levelCounter_[level]; }

void Manifest::setLevelCount(int level, int count) {
  if (levelCounter_.empty() || level == levelCounter_.size()) {
    levelCounter_.push_back(count);
  } else {
    levelCounter_[level] = count;
  }
  FlushManifest();
}

void Manifest::RecoverLevelInfo() {
  manifestReader_.open(filePath_);
  std::string line, level, count;
  while (getline(manifestReader_, line)) {
    splitKV(line, level, count);
    levelCounter_.push_back(std::stoi(count));
  }
  if (levelCounter_.empty())
    setLevelCount(0, 0);
  manifestReader_.close();
}

void Manifest::FlushManifest() {
  manifestWriter_.open(filePath_);
  for (int level = 0; level < levelCounter_.size(); ++level) {
    manifestWriter_ << level << delimiter << levelCounter_[level] << "\n";
  }

  manifestWriter_.flush();
  manifestWriter_.close();
}
