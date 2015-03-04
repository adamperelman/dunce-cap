#ifndef __RELATION_H__
#define __RELATION_H__

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <memory>
#include <unordered_set>
#include <stack>
#include <boost/functional/hash.hpp>

class TrieNode {
public:
  TrieNode(const std::string& attr) : attr_(attr), values_(new std::vector<int>) {}
  TrieNode(const std::string& attr, std::vector<int>* values) : attr_(attr), values_(values), children_(values->size()) {}

  static TrieNode* FromFile(const std::string& filename, std::vector<std::string> attrs);

  static TrieNode* PairwiseJoin(const TrieNode* r1,
                                const TrieNode* r2);

  static int PairwiseCount(const TrieNode* parent,
                           const TrieNode* child);

  std::vector<std::string> attrs() const;

  void InsertTuple(std::vector<int>::iterator tuple_start,
                   std::vector<int>::iterator tuple_end,
                   std::vector<std::string>::iterator attr_start,
                   std::vector<std::string>::iterator attr_end);

  void AppendTuple(std::vector<int>::const_iterator tuple_start,
                   std::vector<int>::const_iterator tuple_end,
                   std::vector<std::string>::const_iterator attr_start,
                   std::vector<std::string>::const_iterator attr_end);

  int size() const; // TODO change this to cache size in each trienode
  bool contains(const std::vector<int>& tuple) const;

  // attr: the attribute to bind to a value
  // val: the value it should get bound to
  const TrieNode* MatchingNode(const std::string& attr, int val) const;

  const std::vector<int>* values() const { return values_.get(); }
  const std::string& attr() const { return attr_; }
  const std::vector<std::unique_ptr<TrieNode>>& children() const { return children_; }
  void AddChildNode(int value, TrieNode* child_ptr);

  std::pair<std::vector<int>, std::vector<int>> OriginalToJoinedIndexes(const TrieNode* other) const;
  std::vector<int> DisjointAttributeIndexes(const TrieNode* other) const;
  std::pair<std::vector<int>, std::vector<int>> SharedAttributeIndexes(const TrieNode* other) const;
  std::vector<std::string> JoinedAttributes(const TrieNode* other) const;

  TrieNode* LeftSemijoin(const TrieNode* other) const;

  class const_iterator {
  public:
    const_iterator(const TrieNode* root);
    const_iterator& operator++();
    friend bool operator==(const const_iterator& a, const const_iterator& b);
    friend bool operator!=(const const_iterator& a, const const_iterator& b);
    const std::vector<int>& operator*() const;
    const std::vector<int>* operator->() const;

  private:
    std::stack<std::pair<const TrieNode*, int>> node_indexes_;
    std::vector<int> current_tuple_;
  };

  const_iterator begin() const;
  const_iterator end() const;

private:
  std::string attr_;
  std::unique_ptr<std::vector<int>> values_;
  std::vector<std::unique_ptr<TrieNode>> children_;

  friend std::ostream& operator<<(std::ostream& os, const TrieNode& rel);
};

#endif /* __RELATION_H__ */
