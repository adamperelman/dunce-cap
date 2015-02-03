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
  TrieNode() {}
  TrieNode(const std::vector<int>& values) : values_(values), children_(values.size()) {
  }

  void InsertTuple(const std::vector<int>& tuple, std::vector<int>::iterator start);
  std::vector<std::vector<int>> MakeTuples() const;

  int size() const;
  bool contains(const std::vector<int>& tuple) const;
  const std::vector<int>& MatchingValues(const std::string& attr,
                                         const std::unordered_map<std::string, int>& bound_attrs) const;

  const std::vector<int>& values() const { return values_; }
  const std::vector<std::unique_ptr<TrieNode>>& children() const { return children_; }

private:
  std::vector<int> values_;
  std::vector<std::unique_ptr<TrieNode>> children_;
};

class Relation {

public:
  Relation(const std::string& filename, std::string relation_name, std::vector<std::string> attrs);
 Relation(std::vector<std::string> attrs, TrieNode* root) : attrs_(attrs), root_(root) {}
  ~Relation() {}

  const std::vector<int>& MatchingValues(const std::string& attr,
                                         const std::unordered_map<std::string, int>& bound_attrs) const;

  bool ContainsAttributes(const std::set<std::string>& attrs) const;

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
