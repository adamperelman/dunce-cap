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
  TrieNode() {};
  TrieNode(const std::string& attr) : attr_(attr) {}
  TrieNode(const std::string& attr, const std::vector<int>& values) : attr_(attr), values_(values), children_(values.size()) {}

  void InsertTuple(std::vector<int>::iterator tuple_start,
                   std::vector<int>::iterator tuple_end,
                   std::vector<std::string>::iterator attr_start,
                   std::vector<std::string>::iterator attr_end);
  std::vector<std::vector<int>> MakeTuples() const;

  int size() const; // TODO change this to cache size in each trienode
  bool contains(const std::vector<int>& tuple) const;
  const std::vector<int>* MatchingValues(const std::string& attr,
                                         const std::unordered_map<std::string, int>& bound_attrs) const;

  const std::vector<int>& values() const { return values_; }
  const std::vector<std::unique_ptr<TrieNode>>& children() const { return children_; }
  void set_attr(const std::string& attr) { attr_ = attr; }
  void AddChildNode(int value, TrieNode* child_ptr);

private:
  std::string attr_;
  std::vector<int> values_;
  std::vector<std::unique_ptr<TrieNode>> children_;
};

class Relation {

public:
  Relation(const std::string& filename, std::string relation_name, std::vector<std::string> attrs);
  Relation(std::vector<std::string> attrs, TrieNode* root) : attrs_(attrs), root_(root) {}
  ~Relation() {}

  const std::vector<int>* MatchingValues(const std::string& attr,
                                         const std::unordered_map<std::string, int>& bound_attrs) const;

  bool ContainsAttribute(const std::string& attr) const;

  // Slow!! Only for testing / printing.
  std::vector<std::vector<int>> MakeTuples() const;

  int size() const;
  bool contains(const std::vector<int>& tuple) const;
  const std::vector<std::string>& attrs() const;
  std::string name() const { return relation_name_; }

private:
  std::vector<std::string> attrs_;
  std::string relation_name_;
  std::unique_ptr<TrieNode> root_;

  friend std::ostream& operator<<(std::ostream& os, const Relation& rel);
};

#endif /* __DATABASE_H__ */
