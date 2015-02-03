#include "relation.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <map>
#include <algorithm>
#include <memory>

using namespace std;

Relation::Relation(const string& filename, string relation_name, vector<string> attrs): attrs_(attrs), relation_name_(relation_name), root_(new TrieNode()) {

  vector<pair<string, int>> attrs_with_indexes;
  for (int i = 0; i < attrs_.size(); i++) {
    attrs_with_indexes.push_back(make_pair(attrs_[i], i));
  }
  sort(attrs_with_indexes.begin(), attrs_with_indexes.end());
  sort(attrs_.begin(), attrs_.end());
  root_->set_attr(attrs_[0]);

  ifstream ifs(filename);

  string line;
  while (getline(ifs, line)) {
    istringstream iss(line);
    int temp;
    vector<int> tuple;
    while (iss >> temp) {
      tuple.push_back(temp);
    }

    vector<int> ordered_tuple;
    for (int i = 0; i < tuple.size(); i++) {
      int index = attrs_with_indexes[i].second;
      ordered_tuple.push_back(tuple[index]);
    }

    if (ordered_tuple.size() != attrs.size()) {
      throw runtime_error("tuple length does not match attributes length");
    }
    root_->InsertTuple(ordered_tuple.begin(), ordered_tuple.end(), attrs_.begin(), attrs_.end());
  }
}

bool Relation::ContainsAttribute(const string& attr) const {
  return binary_search(attrs_.begin(), attrs_.end(), attr);
}

const vector<int>* Relation::MatchingValues(const string& attr,
                                            const unordered_map<string, int>& bound_attrs) const {
  return root_->MatchingValues(attr, bound_attrs);
}

const vector<string>& Relation::attrs() const {
  return attrs_;
}

int Relation::size() const {
  return root_->size();
}

bool Relation::contains(const vector<int>& tuple) const {
  return root_->contains(tuple);
}

vector<vector<int>> Relation::MakeTuples() const {
  return root_->MakeTuples();
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

vector<vector<int>> TrieNode::MakeTuples() const {
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

int TrieNode::size() const {
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

const vector<int>* TrieNode::MatchingValues(const string& attr,
                                            const unordered_map<string, int>& bound_attrs) const {
  if (attr == attr_) {
    return &values_;
  }

  assert(bound_attrs.count(attr_));

  auto val_ptr = lower_bound(values_.begin(), values_.end(), bound_attrs.at(attr_));
  if (val_ptr == values_.end() || *val_ptr != bound_attrs.at(attr_)) {
    return nullptr;
  }

  int index = val_ptr - values_.begin();
  return children_[index]->MatchingValues(attr, bound_attrs);
}

void TrieNode::InsertTuple(vector<int>::iterator tuple_start,
                           vector<int>::iterator tuple_end,
                           vector<string>::iterator attr_start,
                           vector<string>::iterator attr_end) {
  assert(tuple_end - tuple_start == attr_end - attr_start);
  assert(attr_ == *attr_start);

  auto val_ptr = lower_bound(values_.begin(), values_.end(), *tuple_start);
  int index = val_ptr - values_.begin();

  if (val_ptr == values_.end() || *val_ptr != *tuple_start) {
    values_.insert(val_ptr, *tuple_start);

    if (tuple_start+1 != tuple_end) {
      children_.insert(children_.begin()+index,
                       unique_ptr<TrieNode>(new TrieNode(*(attr_start+1))));
    } else {
      children_.insert(children_.begin()+index, unique_ptr<TrieNode>(nullptr));
    }
  }

  if (tuple_start+1 != tuple_end) {
    children_[index]->InsertTuple(tuple_start+1, tuple_end, attr_start+1, attr_end);
  }
}

bool TrieNode::contains(const vector<int>& tuple) const {
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


void TrieNode::AddChildNode(int value, TrieNode* child_ptr) {
  assert(values_.empty() || value > values_.back());
  values_.push_back(value);
  children_.push_back(unique_ptr<TrieNode>(child_ptr));
  assert(values_.size() == children_.size());
}
