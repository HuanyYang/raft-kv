#ifndef RAFT_KV_MANIFEST_H
#define RAFT_KV_MANIFEST_H

#include "fstream"
#include "string"
#include "vector"
#include <unistd.h>

// 管理SST的元信息

class Manifest {
  explicit Manifest(const std::string &filePath) : filePath_(filePath) {
    levelCounter_.push_back(0);
    if (access(filePath_.c_str(), F_OK) != 0) {
      manifestWriter_.open(filePath_);
    }
    void RecoverLevel();
  }

  Manifest(const Manifest &) = delete;
  Manifest &operator=(const Manifest &) = delete;
  ~Manifest();

private:
  void RecoverLevel();

  std::string filePath_;
  std::vector<int> levelCounter_;
  std::ofstream manifestWriter_;
  std::ifstream manifestReader_;
};

Manifest::~Manifest() {
  if (manifestWriter_.is_open())
    manifestWriter_.close();
  if (manifestReader_.is_open())
    manifestReader_.close();
}
void Manifest::RecoverLevel() {
  manifestReader_.open(filePath_);
  std::string line, level, count;
  while (getline(manifestReader_, line)) {
  }
}

#endif // RAFT_KV_MANIFEST_H
