#ifndef RAFT_KV_SKIPLIST_H
#define RAFT_KV_SKIPLIST_H

#include "cstdlib"
#include "cstring"
#include "iostream"

template <typename K, typename V> class SkipList;

template <typename K, typename V> class Node {
  friend class SkipList<K, V>;

public:
  Node() = default;
  Node(K k, V v, int level);
  ~Node();

  K getKey() const;
  V getValue() const;
  void setValue(V newV);

private:
  K key;
  V value;
  int nodeLevel;
  Node<K, V> **forward;
};

template <typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level)
    : K(k), V(v), nodeLevel(level) {
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
  explicit SkipList(int maxlevel);
  ~SkipList();

  Node<K, V> *createNode(K, V, int);
  bool searchElement(K);
  bool insertElement(K, V);
  void deleteElement(K);
  int size();
  void displayList();

private:
  int getRandomLevel();

  int maxLevel;
  int skipListLevel;
  Node<K, V> *header;
  int elementCount;
};

template <typename K, typename V> SkipList<K, V>::SkipList(int maxlevel) {
  maxLevel = maxlevel;
  skipListLevel = 0;
  elementCount = 0;
  K k;
  V v;
  header = new Node<K, V>(k, v, maxlevel);
}

template <typename K, typename V> SkipList<K, V>::~SkipList() { delete header; }

template <typename K, typename V>
Node<K, V> *SkipList<K, V>::createNode(const K k, const V v, int level) {
  auto *node = new Node<K, V>(k, v, level);
  return node;
}

template <typename K, typename V> bool SkipList<K, V>::searchElement(K key) {
  Node<K, V> *current = header;
  for (int i = skipListLevel; i >= 0; --i) {
    while (current->forward[i] && current->forward[i]->getKey() < key) {
      current = current->forward[i];
    }
  }
  current = current->forward[0];
  if (current && current->getKey() == key)
    return true;
  return false;
}

template <typename K, typename V>
bool SkipList<K, V>::insertElement(K key, V value) {
  Node<K, V> *current = header;
  Node<K, V> *prevNodes[maxLevel + 1];

  for (int i = skipListLevel; i >= 0; --i) {
    while (current->forward[i] && current->forward[i]->getKey() < key) {
      current = current->forward[i];
    }
    prevNodes[i] = current;
  }
  current = current->forward[0];
  if (!current || current->getKey() != key) {
    int randomLevel = getRandomLevel();
    if (randomLevel > maxLevel) {
      for (int i = randomLevel + 1; i < maxLevel; ++i) {
        prevNodes[i] = header;
      }
      skipListLevel = randomLevel;
    }
    Node<K, V> *newNode = createNode(key, value, randomLevel);
    for (int i = 0; i <= randomLevel; ++i) {
      newNode->forward[i] = prevNodes[i]->forward[i];
      prevNodes[i]->forward[i] = newNode;
    }
    ++elementCount;
    return true;
  }
  return false;
}
template <typename K, typename V> void SkipList<K, V>::deleteElement(K key) {
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
    --elementCount;
  }
}

template <typename K, typename V> int SkipList<K, V>::size() {
  return elementCount;
}

template <typename K, typename V> void SkipList<K, V>::displayList() {
  cout << "displayList:" << endl;
  for (int i = 0; i <= skipListLevel; ++i) {
    Node<K, V> *node = this->header->forward[i];
    cout << "Level " << i << ": ";
    while (node) {
      cout << "[" << node->getKey() << "," << node->getValue() << "];";
      node = node->forward[i];
    }
    cout << endl;
  }
}
template <typename K, typename V> int SkipList<K, V>::getRandomLevel() {
  int level = 1;
  while (rand() % 2 && level < maxLevel)
    ++level;
  return level;
}

#endif // RAFT_KV_SKIPLIST_H
