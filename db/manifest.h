#ifndef RAFT_KV_MANIFEST_H
#define RAFT_KV_MANIFEST_H

#include "fstream"
#include "iostream"
#include "string"
#include "unistd.h"
#include "vector"

// 管理SST的元信息

class Manifest {
public:
  explicit Manifest(const std::string &filePath);

  Manifest(const Manifest &) = delete;
  Manifest &operator=(const Manifest &) = delete;
  ~Manifest();

  void RecoverManifest();
  void ShowManifest();

  int getLeveCount(int level);
  void setLevelCount(int level, int count);

private:
  void RecoverLevelInfo();
  void FlushManifest();

  std::string filePath_;
  std::vector<int> levelCounter_;
  std::ofstream manifestWriter_;
  std::ifstream manifestReader_;
};

#endif // RAFT_KV_MANIFEST_H
