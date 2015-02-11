#ifndef __RELATION_H__
#define __RELATION_H__

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <memory>
#include <unordered_map>

class TrieNode {
public:
  TrieNode(const std::string& attr) : attr_(attr), values_(new std::vector<int>) {}
  TrieNode(const std::string& attr, std::vector<int>* values) : attr_(attr), values_(values), children_(values->size()) {}

  static TrieNode* FromFile(const std::string& filename, std::vector<std::string> attrs);

  bool ContainsAttribute(const std::string& attr) const;

  std::vector<std::string> attrs() const;

  void InsertTuple(std::vector<int>::iterator tuple_start,
                   std::vector<int>::iterator tuple_end,
                   std::vector<std::string>::iterator attr_start,
                   std::vector<std::string>::iterator attr_end);
  std::vector<std::vector<int>> MakeTuples() const;

  int size() const; // TODO change this to cache size in each trienode
  bool contains(const std::vector<int>& tuple) const;
  const std::vector<int>* MatchingValues(const std::string& attr,
                                         const std::unordered_map<std::string, int>& bound_attrs) const;

  const std::vector<int>& values() const { return *values_; }
  const std::vector<std::unique_ptr<TrieNode>>& children() const { return children_; }
  void AddChildNode(int value, TrieNode* child_ptr);

private:
  std::string attr_;
  std::unique_ptr<std::vector<int>> values_;
  std::vector<std::unique_ptr<TrieNode>> children_;

  friend std::ostream& operator<<(std::ostream& os, const TrieNode& rel);
};

#endif /* __RELATION_H__ */
