#include "relation.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <map>

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
    tuples_.insert(tuple);
  }
}

Relation::Relation(vector<string> attrs): attrs_(attrs) {
}

Relation::Relation(vector<string> attrs, set<vector<int>> tuples): attrs_(attrs), tuples_(tuples) {
}

void Relation::AddTuple(vector<int> tuple) {
  tuples_.insert(tuple);
}

Relation* Relation::Intersect(const vector<Relation*>& relations) {
  for (const Relation* relation : relations) {
    assert(relation->attrs().size() == 1);
  }

  set<vector<int>> intersection(relations.at(0)->tuples_);
  for (int i = 1; i < relations.size(); i++) {
    set<vector<int>> temp;
    set_intersection(intersection.begin(), intersection.end(),
                     relations[i]->tuples_.begin(), relations[i]->tuples_.end(),
                     inserter(temp, temp.begin()));
    intersection = temp;
  }

  return new Relation(relations[0]->attrs(), intersection);
}

Relation* Relation::Project(const set<string>& attrs) const {
  vector<bool> include_column;
  vector<string> ordered_projected_columns;
  for (int i = 0; i < attrs_.size(); i++) {
    if (attrs.find(attrs_[i]) != attrs.end()) {
      include_column.push_back(true);
      ordered_projected_columns.push_back(attrs_[i]);
    } else {
      include_column.push_back(false);
    }
  }

  Relation* projection = new Relation(ordered_projected_columns);
  for (const vector<int>& t : tuples_) {
    vector<int> new_tuple;
    for (int i = 0; i < t.size(); i++) {
      if (include_column[i]) {
        new_tuple.push_back(t[i]);
      }
    }
    projection->AddTuple(new_tuple);
  }
  return projection;
}

Relation* Relation::LeftSemiJoin(const vector<int>& tuple, const vector<string>& attrs) const {
  map<int,int> tuple_index_to_val;
  for (int i = 0; i < attrs_.size(); ++i) {
    auto it = find(attrs.begin(), attrs.end(), attrs_[i]);
    if (it != attrs.end()) {
      tuple_index_to_val[i] = tuple[it - attrs.begin()];
    }
  }

  Relation* result = new Relation(attrs_);
  for (const vector<int>& t : tuples_)  {
    bool insert_tuple = true;
    for (int i = 0 ; i < t.size(); ++i) {
      if (tuple_index_to_val.count(i) && tuple_index_to_val[i] != t[i]) {
        insert_tuple = false;
        break;
      }
    }
    if (insert_tuple) {
      result->AddTuple(t);
    }
  }
  return result;
}


Relation* Relation::CartesianProduct(const vector<int>& tuple, const vector<string>& attrs) const {
  vector<string> concat_attrs(attrs_);
  concat_attrs.insert(concat_attrs.end(), attrs.begin(), attrs.end());
  Relation* result = new Relation(concat_attrs);

  for (vector<int> t : tuples_) {
    t.insert(t.end(), tuple.begin(), tuple.end());
    result->AddTuple(t);
  }
  return result;
}

bool Relation::ContainsAttributes(const set<string>& attrs) const {
  for (const string& at : attrs_) {
    if (attrs.count(at)) {
      return true;
    } 
  }
  return false;
}

Relation* Relation::SortedByAttributes() {
  // Sort the attributes together with their original indexes
  // so we can later sort each tuple in the same order.
  vector<pair<string, int>> attrs_with_indexes;
  for (int i = 0; i < attrs_.size(); i++) {
    attrs_with_indexes.push_back(make_pair(attrs_[i], i));
  }
  sort(attrs_with_indexes.begin(), attrs_with_indexes.end());

  vector<string> attrs_copy(attrs_);
  sort(attrs_copy.begin(), attrs_copy.end());
  Relation* result = new Relation(attrs_copy);

  for (set<vector<int>>::iterator it = tuples_.begin(); it != tuples_.end(); ++it) {
    vector<int> tuple;
    for (int i = 0; i < it->size(); i++) {
      int index = attrs_with_indexes[i].second;
      tuple.push_back(it->at(index));
    }

    result->AddTuple(tuple);
  }

  return result;
}

const vector<string>& Relation::attrs() const {
  return attrs_;
}

int Relation::size() const {
  return tuples_.size();
}

bool Relation::contains(const vector<int>& tuple) const {
  return tuples_.find(tuple) != tuples_.end();
}
