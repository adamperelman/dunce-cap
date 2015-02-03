#include "relation.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <map>
#include <algorithm>
#include <memory>

using namespace std;

Relation::Relation(const string& filename, string relation_name, vector<string> attrs): attrs_(attrs), relation_name_(relation_name) {

  ifstream ifs(filename);

  string line;
  while (getline(ifs, line)) {
    istringstream iss(line);
    int temp;
    vector<int> tuple;
    while (iss >> temp) {
      tuple.push_back(temp);
    }
    if (tuple.size() != attrs.size()) {
      throw runtime_error("tuple length does not match attributes length");
    }
    root_.InsertTuple(tuple, tuple.begin());
  }
}

bool Relation::ContainsAttributes(const set<string>& attrs) const {
  for (const string& at : attrs_) {
    if (attrs.count(at)) {
      return true;
    }
  }
  return false;
}

const vector<string>& Relation::attrs() const {
  return attrs_;
}

int Relation::size() const {
  return root_.size();
}

bool Relation::contains(const vector<int>& tuple) const {
  return root_.contains(tuple);
}

vector<vector<int>> Relation::MakeTuples() const {
  return root_.MakeTuples();
}

ostream& operator<<(ostream& os, const Relation& rel) {
  for (const string& attr : rel.attrs()) {
    cout << attr << "\t";
  }

  for (const vector<int>& tuple : rel.MakeTuples()) {
    for (int val : tuple) {
      cout << val << "\t";
    }
    cout << endl;
  }

  return os;
}

vector<vector<int>> Relation::TrieNode::MakeTuples() const {
  vector<vector<int>> tuples;

  for (int i = 0; i < values_.size(); i++) {
    int val = values_[i];
    const unique_ptr<TrieNode>& child_ptr = children_[i];
    if (!child_ptr) {
      tuples.push_back(vector<int>({val}));
    } else {
      for (vector<int> t : child_ptr->MakeTuples()) {
        t.insert(t.begin(), val);
        tuples.push_back(t);
      }
    }
  }

  return tuples;
}

int Relation::TrieNode::size() const {
  int result = 0;
  for (int i = 0; i < values_.size(); i++) {
    const unique_ptr<TrieNode>& child_ptr = children_[i];
    if (child_ptr) {
      result += child_ptr->size();
    } else {
      result += 1;
    }
  }
  return result;
}

void Relation::TrieNode::InsertTuple(const vector<int>& tuple, vector<int>::iterator start) {
  auto val_ptr = lower_bound(values_.begin(), values_.end(), *start);
  int index = val_ptr - values_.begin();

  if (*val_ptr != *start) {
    values_.insert(val_ptr, *start);

    if (start+1 != tuple.end()) {
      children_.insert(children_.begin()+index,
                       unique_ptr<TrieNode>(new TrieNode()));
    } else {
      children_.insert(children_.begin()+index, unique_ptr<TrieNode>(nullptr));
    }
  }

  if (start+1 != tuple.end()) {
    children_[index]->InsertTuple(tuple, start+1);
  }
}

bool Relation::TrieNode::contains(const vector<int>& tuple) const {
  if (tuple.empty()) {
    throw runtime_error("tuple passed to contains() is too short");
  }

  auto val_ptr = lower_bound(values_.begin(), values_.end(), tuple[0]);
  if (*val_ptr != tuple[0]) {
    // lower_bound() didn't find a match.
    return false;
  }

  int index = val_ptr - values_.begin();
  const auto& child_ptr = children_[index];

  if (child_ptr) {
    vector<int> remaining_tuple(tuple.begin()+1, tuple.end());
    return child_ptr->contains(remaining_tuple);
  }

  if (tuple.size() == 1) {
    return true;
  }

  throw runtime_error("tuple passed to contains() is too long");
}
