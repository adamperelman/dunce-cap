#include "relation.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <map>
#include <algorithm>
#include <memory>

using namespace std;

struct tuple_hash {
  size_t operator()(const std::vector<int>& tuple) const {
    return boost::hash_range(tuple.begin(), tuple.end());
  }
};

TrieNode* TrieNode::FromFile(const string& filename, vector<string> attrs) {
  vector<pair<string, int>> attrs_with_indexes;
  for (int i = 0; i < attrs.size(); i++) {
    attrs_with_indexes.push_back(make_pair(attrs[i], i));
  }
  sort(attrs_with_indexes.begin(), attrs_with_indexes.end());
  sort(attrs.begin(), attrs.end());

  TrieNode* root = new TrieNode(attrs[0]);

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

    root->InsertTuple(ordered_tuple.begin(), ordered_tuple.end(), attrs.begin(), attrs.end());
  }
  return root;
}

bool TrieNode::ContainsAttribute(const string& attr) const {
  if (attr_ == attr) {
    return true;
  }

  if (children_.empty() || !children_[0]) {
    return false;
  }

  return children_[0]->ContainsAttribute(attr);
}

vector<string> TrieNode::attrs() const {
  if (children_.empty() || !children_[0]) {
    return vector<string>({attr_});
  }

  vector<string> node_attrs = children_[0]->attrs();
  node_attrs.insert(node_attrs.begin(), attr_);
  return node_attrs;
}

ostream& operator<<(ostream& os, const TrieNode& rel) {
  for (const string& attr : rel.attrs()) {
    cout << attr << "\t";
  }

  cout << endl;

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

  for (int i = 0; i < values_->size(); i++) {
    int val = values_->at(i);
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
  for (int i = 0; i < values_->size(); i++) {
    const unique_ptr<TrieNode>& child_ptr = children_[i];
    if (child_ptr) {
      result += child_ptr->size();
    } else {
      result += 1;
    }
  }
  return result;
}

const TrieNode* TrieNode::MatchingNode(const std::string& attr, int val) const {
  if (attr != attr_) {
    // We're not binding our node's attribute yet, so all our values match.
    return this;
  }

  // We're binding this node's attribute now, so we only want a subset of our values.
  auto val_ptr = lower_bound(values_->begin(), values_->end(), val);
  if (val_ptr == values_->end() || *val_ptr != val) {
    // We don't have a matching value.
    return nullptr;
  }

  int index = val_ptr - values_->begin();
  return children_[index].get();
}

void TrieNode::InsertTuple(vector<int>::iterator tuple_start,
                           vector<int>::iterator tuple_end,
                           vector<string>::iterator attr_start,
                           vector<string>::iterator attr_end) {
  assert(tuple_end - tuple_start == attr_end - attr_start);
  assert(attr_ == *attr_start);

  auto val_ptr = lower_bound(values_->begin(), values_->end(), *tuple_start);
  int index = val_ptr - values_->begin();

  if (val_ptr == values_->end() || *val_ptr != *tuple_start) {
    values_->insert(val_ptr, *tuple_start);

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

void TrieNode::AppendTuple(vector<int>::const_iterator tuple_start,
                           vector<int>::const_iterator tuple_end,
                           vector<string>::const_iterator attr_start,
                           vector<string>::const_iterator attr_end) {

  assert(tuple_end - tuple_start == attr_end - attr_start);
  assert(attr_ == *attr_start);

  if (values_->empty() || *tuple_start > values_->back()) {
    values_->push_back(*tuple_start);
    if (tuple_start+1 != tuple_end) {
      children_.push_back(unique_ptr<TrieNode>(new TrieNode(*(attr_start+1))));
    } else {
      children_.push_back(unique_ptr<TrieNode>(nullptr));
    }
  } else if (*tuple_start != values_->back()) {
    throw runtime_error("Cannot call AppendTuple for tuple out of nondecreasing order");
  }

  if (tuple_start+1 != tuple_end) {
    children_.back()->AppendTuple(tuple_start+1, tuple_end, attr_start+1, attr_end);
  }
}

bool TrieNode::contains(const vector<int>& tuple) const {
  if (tuple.empty()) {
    throw runtime_error("tuple passed to contains() is too short");
  }

  auto val_ptr = lower_bound(values_->begin(), values_->end(), tuple[0]);
  if (*val_ptr != tuple[0]) {
    // lower_bound() didn't find a match.
    return false;
  }

  int index = val_ptr - values_->begin();
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
  assert(values_->empty() || value > values_->back());
  values_->push_back(value);
  children_.push_back(unique_ptr<TrieNode>(child_ptr));
  assert(values_->size() == children_.size());
}

pair<vector<int>, vector<int>> TrieNode::SharedAttributeIndexes(const TrieNode* other) const {
  int this_index = 0;
  int other_index = 0;

  vector<string> this_attrs = attrs();
  vector<string> other_attrs = other->attrs();

  pair<vector<int>, vector<int>> result;
  while (this_index < this_attrs.size() && other_index < other_attrs.size()) {
    if (this_attrs[this_index] == other_attrs[other_index]) {
      result.first.push_back(this_index++);
      result.second.push_back(other_index++);
    } else if (this_attrs[this_index] < other_attrs[other_index]) {
      this_index++;
    } else {
      other_index++;
    }
  }
  return result;
}

TrieNode* TrieNode::LeftSemijoin(const TrieNode* other) const {
  pair<vector<int>, vector<int>> shared_attr_indexes = SharedAttributeIndexes(other);
  unordered_set<vector<int>, tuple_hash> other_tuples;
  vector<int> buffer(shared_attr_indexes.first.size());

  for (TrieNode::const_iterator it = other->begin(); it != other->end(); ++it) {
    for(int i=0; i<buffer.size(); ++i) {
      buffer[i] = it->at(shared_attr_indexes.second.at(i));
    }
    other_tuples.insert(buffer);
  }

  TrieNode* new_relation = new TrieNode(attr_);
  vector<string> this_attrs = attrs();

  for (TrieNode::const_iterator it = begin(); it != end(); ++it) {
     for(int i=0; i<buffer.size(); ++i) {
      buffer[i] = it->at(shared_attr_indexes.first.at(i));
    }
    if (other_tuples.count(buffer)) {
      new_relation->AppendTuple(it->begin(),
                                it->end(),
                                this_attrs.begin(),
                                this_attrs.end());
    }
  }
  return new_relation;
}

TrieNode* TrieNode::Join(const TrieNode* other) const {
  pair<vector<int>, vector<int>> shared_attr_indexes = SharedAttributeIndexes(other);
  vector<int> other_disjoint_attr_indexes = DisjointAttributeIndexes(other);

  unordered_map<vector<int>, vector<vector<int>>, tuple_hash> other_tuples;

  vector<int> key_buffer(shared_attr_indexes.first.size());
  vector<int> val_buffer(other_disjoint_attr_indexes.size());

  for (TrieNode::const_iterator it = other->begin();
       it != other->end();
       ++it) {
    for(int i = 0; i < key_buffer.size(); ++i) {
      key_buffer[i] = it->at(shared_attr_indexes.second.at(i));
    }
    for (int i = 0; i < val_buffer.size(); ++i) {
      val_buffer[i] = it->at(other_disjoint_attr_indexes.at(i));
    }
    other_tuples[key_buffer].push_back(val_buffer);
  }

  vector<string> joined_attrs = JoinedAttributes(other);
  vector<int> joined_tuple_buffer = vector<int>(joined_attrs.size());
  pair<vector<int>, vector<int>> orig_to_joined_indexes = OriginalToJoinedIndexes(other);

  TrieNode* new_relation = new TrieNode(joined_attrs[0]);

  for (TrieNode::const_iterator it = begin(); it != end(); ++it) {
    for (int i = 0; i < key_buffer.size(); ++i) {
      key_buffer[i] = it->at(shared_attr_indexes.first.at(i));
    }
    if (other_tuples.count(key_buffer)) {
      // Set the values in our buffer that come from this relation's tuple.
      for (int i = 0; i < it->size(); ++i) {
        joined_tuple_buffer[orig_to_joined_indexes.first[i]] = it->at(i);
      }
      for (const vector<int>& matching_tuple : other_tuples[key_buffer]) {
        // Set the values in our buffer that come from the other relation's tuple.
        for (int i = 0; i < matching_tuple.size(); ++i) {
          joined_tuple_buffer[orig_to_joined_indexes.second[i]] = matching_tuple[i];
        }

        // TODO: InsertTuple is slow!
        new_relation->InsertTuple(joined_tuple_buffer.begin(),
                                  joined_tuple_buffer.end(),
                                  joined_attrs.begin(),
                                  joined_attrs.end());
      }
    }
  }

  return new_relation;
}

TrieNode::const_iterator::const_iterator(const TrieNode* root) {
  const TrieNode* current_node = root;
  while (current_node) {
    // TODO: this assumes that all nodes have size > 0; is this true??
    node_indexes_.push(make_pair(current_node, 0));
    current_tuple_.push_back(current_node->values()->at(0));
    current_node = current_node->children().at(0).get();
  }
}

TrieNode::const_iterator& TrieNode::const_iterator::operator++() {
  // Pop off nodes with no more values.
  while (node_indexes_.top().second == node_indexes_.top().first->values()->size() - 1) {
    node_indexes_.pop();
    current_tuple_.pop_back();
    if (node_indexes_.empty()) {
      // We're done iterating.
      return *this;
    }
  }

  // Increment our current node's index.
  node_indexes_.top().second++;
  current_tuple_.back() = node_indexes_.top().first->values()->at(node_indexes_.top().second);

  // Walk down to child (if possible).
  while (const TrieNode* child = node_indexes_.top().first->children()[node_indexes_.top().second].get()) {
    node_indexes_.push(make_pair(child, 0));
    current_tuple_.push_back(child->values()->at(0));
  }

  return *this;
}


bool operator==(const TrieNode::const_iterator& a, const TrieNode::const_iterator& b) {
  // For now, we just consider them to be equal if their stacks are empty
  return a.node_indexes_.empty() && b.node_indexes_.empty();
}

bool operator!=(const TrieNode::const_iterator& a, const TrieNode::const_iterator& b) {
  return !(a==b);
}

const vector<int>& TrieNode::const_iterator::operator*() const {
  return current_tuple_;
}

const vector<int>* TrieNode::const_iterator::operator->() const {
  return &current_tuple_;
}

TrieNode::const_iterator TrieNode::begin() const {
  return TrieNode::const_iterator(this);
}

TrieNode::const_iterator TrieNode::end() const {
  return TrieNode::const_iterator(NULL);
}
