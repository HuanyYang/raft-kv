#include "db/leptdb.h"

int main() {
  int num = 200;
  Leptdb leptdb("myLeptDB");
  while (num--) {
    std::cout << "---------------" << num << "---------------" << std::endl;
    leptdb.Put("8", "代码");
    leptdb.Put("2", "er二");
    leptdb.Put("9", "学习");
    leptdb.Put("6", "运行");
    leptdb.Put("1", "首先");
    leptdb.Put("6", "六变");
    leptdb.Put("3", "算法");
    leptdb.Put("7", "结果出现");
    leptdb.Put("5", "sdf");
    leptdb.Delete("6");
    std::cout << "size: " << leptdb.getMemtableSize() << std::endl;
    leptdb.ShowManifest();
  }
}
