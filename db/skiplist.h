#ifndef RAFT_KV_SKIPLIST_H
#define RAFT_KV_SKIPLIST_H

#include "cstring"
#include "ctime"
#include "iostream"
#include "random"

const int defaultLevel = 10;

template <typename K, typename V> class SkipList;
using memtable = SkipList<std::string, std::string>;

template <typename K, typename V> class Node {
  friend class SkipList<K, V>;

public:
  Node() = default;
  Node(K k, V v, int level);
  ~Node();

  K getKey() const;
  V getValue() const;
  void setValue(V value);

private:
  K key;
  V value;
  int nodeLevel;
  Node<K, V> **forward;
};

template <typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level)
    : key(k), value(v), nodeLevel(level) {
  this->forward = new Node<K, V> *[level + 1];
  memset(this->forward, 0, sizeof(Node<K, V> *) * (level + 1));
}

template <typename K, typename V> Node<K, V>::~Node() { delete[] forward; }

template <typename K, typename V> K Node<K, V>::getKey() const { return key; }

template <typename K, typename V> V Node<K, V>::getValue() const {
  return value;
}

template <typename K, typename V> void Node<K, V>::setValue(V value) {
  this->value = value;
}

template <typename K, typename V> class SkipList {
public:
  explicit SkipList(int maxlevel = defaultLevel);
  SkipList(const SkipList &) = delete;
  SkipList &operator=(const SkipList &) = delete;
  ~SkipList();

  bool searchElement(const K &key, K &value);
  bool insertElement(const K &key, const V &value);
  void deleteElement(const K &key);

  Node<K, V> *Iter();
  void ForwardIter(Node<K, V> *&iter);

  int elementCount();
  void displayList();

  void Clear();

private:
  Node<K, V> *createNode(K, V, int);
  int getRandomLevel();

  int maxLevel;
  int skipListLevel;
  Node<K, V> *header;
  int elementCount_;
};

template <typename K, typename V> SkipList<K, V>::SkipList(int maxlevel) {
  maxLevel = maxlevel;
  skipListLevel = 0;
  elementCount_ = 0;

  K k;
  V v;
  header = new Node<K, V>(k, v, maxlevel);
}

template <typename K, typename V> SkipList<K, V>::~SkipList() {
  Clear();
  delete header;
}

template <typename K, typename V>
Node<K, V> *SkipList<K, V>::createNode(const K k, const V v, int level) {
  auto *node = new Node<K, V>(k, v, level);
  return node;
}

template <typename K, typename V>
bool SkipList<K, V>::searchElement(const K &key, K &value) {
  Node<K, V> *current = header;
  for (int i = skipListLevel; i >= 0; --i) {
    while (current->forward[i] && current->forward[i]->getKey() < key) {
      current = current->forward[i];
    }
  }
  current = current->forward[0];
  if (current && current->getKey() == key) {
    value = current->getValue();
    return true;
  }
  return false;
}

template <typename K, typename V>
bool SkipList<K, V>::insertElement(const K &key, const V &value) {
  Node<K, V> *current = header;
  Node<K, V> *prevNodes[maxLevel + 1];
  memset(prevNodes, 0, sizeof(Node<K, V> *) * (maxLevel + 1));

  for (int i = skipListLevel; i >= 0; --i) {
    while (current->forward[i] && current->forward[i]->getKey() < key) {
      current = current->forward[i];
    }
    prevNodes[i] = current;
  }
  current = current->forward[0];

  if (!current || current->getKey() != key) {
    int randomLevel = getRandomLevel();
    if (randomLevel > skipListLevel) {
      for (int i = skipListLevel + 1; i <= randomLevel; ++i) {
        prevNodes[i] = header;
      }
      skipListLevel = randomLevel;
    }

    Node<K, V> *newNode = createNode(key, value, randomLevel);

    for (int i = 0; i <= randomLevel; ++i) {
      newNode->forward[i] = prevNodes[i]->forward[i];
      prevNodes[i]->forward[i] = newNode;
    }

    ++elementCount_;
  } else {
    // key相同则替换
    current->setValue(value);
  }
  return true;
}

template <typename K, typename V>
void SkipList<K, V>::deleteElement(const K &key) {
  Node<K, V> *current = header;
  Node<K, V> *prevNodes[maxLevel + 1];

  for (int i = skipListLevel; i >= 0; --i) {
    while (current->forward[i] && current->forward[i]->getKey() < key) {
      current = current->forward[i];
    }
    prevNodes[i] = current;
  }
  current = current->forward[0];
  if (current && current->getKey() == key) {
    for (int i = 0; i <= skipListLevel; ++i) {
      if (prevNodes[i]->forward != current)
        break;
      prevNodes[i]->forward[i] = current->forward[i];
    }
    while (skipListLevel > 0 && !header->forward[skipListLevel]) {
      --skipListLevel;
    }
    delete current;
    --elementCount_;
  }
}

template <typename K, typename V> Node<K, V> *SkipList<K, V>::Iter() {
  return header->forward[0];
}

template <typename K, typename V>
void SkipList<K, V>::ForwardIter(Node<K, V> *&iter) {
  if (iter)
    iter = iter->forward[0];
}

template <typename K, typename V> int SkipList<K, V>::elementCount() {
  return elementCount_;
}

template <typename K, typename V> void SkipList<K, V>::displayList() {
  std::cout << "elementCount_: " << elementCount_ << std::endl;
  for (int i = skipListLevel; i >= 0; --i) {
    Node<K, V> *node = this->header->forward[i];
    std::cout << "Level " << i << ": ";
    while (node) {
      std::cout << "[" << node->getKey() << "," << node->getValue() << "];";
      node = node->forward[i];
    }
    std::cout << std::endl;
  }
}

template <typename K, typename V> int SkipList<K, V>::getRandomLevel() {
  static std::default_random_engine engine(time(0));
  static std::uniform_int_distribution<unsigned> ud(1, 100);

  int level = 1;
  while (ud(engine) % 2 && level < maxLevel)
    ++level;
  return level;
}

template <typename K, typename V> void SkipList<K, V>::Clear() {
  Node<K, V> *p = header->forward[0], *latter;
  while (elementCount_-- > 0) {
    latter = p->forward[0];
    delete p;
    p = latter;
  }
  memset(header->forward, 0, sizeof(Node<K, V> *) * (header->nodeLevel + 1));
}

#endif // RAFT_KV_SKIPLIST_H
