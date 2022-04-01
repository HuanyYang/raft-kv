#include "leptdb.h"

int main() {
  Leptdb leptdb("myLeptDB");
  leptdb.ShowWtable();
  leptdb.ShowRtable();
}

// void skipListTest() {
//   SkipList<std::string, std::string> readList(10);
//   readList.displayList();
//   SkipList<std::string, std::string> myList(10);
//   myList.insertElement("8", "代码");
//   myList.insertElement("2", "er二");
//   myList.insertElement("9", "学习");
//   myList.insertElement("6", "运行");
//   myList.insertElement("6", "无无");
//   myList.insertElement("1", "首先");
//   myList.insertElement("3", "算法");
//   myList.insertElement("7", "结果出现");
//   myList.insertElement("5", "sdf");
//   myList.displayList();
// }
