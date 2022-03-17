#include "SkipList.h"
int main() {
  SkipList<int, std::string> myList(10);
  myList.insertElement(8, "代码");
  myList.insertElement(2, "er二");
  myList.insertElement(9, "学习");
  myList.insertElement(19, "运行");
  myList.insertElement(19, "无无");
  myList.insertElement(1, "首先");
  myList.insertElement(3, "算法");
  myList.insertElement(7, "结果出现");
  myList.insertElement(12, "sdf");
  myList.displayList();
}
