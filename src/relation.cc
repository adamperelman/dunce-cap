#include "relation.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <map>
#include <algorithm>
#include <memory>
#include <iterator>

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

Relation::Relation(vector<string> attrs, size_t num_tuples_hint): attrs_(attrs) {
  tuples_.reserve(num_tuples_hint);
}

Relation::Relation(vector<string> attrs, tuple_set tuples): attrs_(attrs), tuples_(tuples) {
}

void Relation::AddTuple(vector<int> tuple) {
  tuples_.insert(tuple);
}

Relation* Relation::Union(const vector<unique_ptr<Relation>>& relations) {
  assert(!relations.empty());

  size_t size_hint = (*max_element(relations.begin(), relations.end()))->size();
  Relation* result = new Relation(relations[0]->attrs(), size_hint);

  for (const unique_ptr<Relation>& relation : relations) {
    assert(relation->attrs() == relations[0]->attrs());
    for (const vector<int>& t : relation->tuples()) {
      result->AddTuple(t);
    }
  }

  return result;
}

bool relation_size_cmp(const Relation* const & rel1, const Relation* const & rel2) {
  return rel1->size() < rel2->size();
}

Relation* Relation::Intersect(const vector<Relation*>& relations) {
  for (const Relation* relation : relations) {
    assert(relation->attrs().size() == 1);
  }

  const Relation* smallest_relation =  *min_element(begin(relations), end(relations), relation_size_cmp);
  tuple_set intersection(smallest_relation->size());
  for (const vector<int>& tuple: smallest_relation->tuples()) {
    bool insert_tuple = true;
    for (const Relation* relation : relations) {
      if (relation != smallest_relation) {
        if (relation->tuples().count(tuple) == 0) {
          insert_tuple = false;
          break;
        }
      }
    }
    if (insert_tuple) {
      intersection.insert(tuple);
    }
  }
  return new Relation(relations[0]->attrs(), intersection);
}

void Relation::PopulateIncludeAndOrderedProjectedCols(const set<string>& attrs, vector<int>& include_column, vector<string>& ordered_projected_columns) const {
  for (int i = 0; i < attrs_.size(); i++) {
    if (attrs.find(attrs_[i]) != attrs.end()) {
      include_column.push_back(i);
      ordered_projected_columns.push_back(attrs_[i]);
    }
  }
}

Relation* Relation::Project(const set<string>& attrs) const {
  vector<int> include_column;
  vector<string> ordered_projected_columns;
  PopulateIncludeAndOrderedProjectedCols(attrs, include_column, ordered_projected_columns);

  // TODO: improve size hint?
  Relation* projection = new Relation(ordered_projected_columns, 0);
  for (const vector<int>& t : tuples_) {
    vector<int> new_tuple(include_column.size());
    for (int i = 0; i < include_column.size(); i++) {
      new_tuple[i] = t[include_column[i]];
    }
    projection->AddTuple(new_tuple);
  }
  return projection;
}

Relation* Relation::LeftSemiJoinAndProject(const vector<int>& tuple, const vector<string>& attrs, const set<string>& projection_attrs) const {
  vector<int> include_column;
  vector<string> ordered_projected_columns;
  PopulateIncludeAndOrderedProjectedCols(projection_attrs, include_column, ordered_projected_columns);

  map<int,int> tuple_index_to_val;
  for (int i = 0; i < attrs_.size(); ++i) {
    auto it = find(attrs.begin(), attrs.end(), attrs_[i]);
    if (it != attrs.end()) {
      tuple_index_to_val[i] = tuple[it - attrs.begin()];
    }
  }

  // TODO: improve size hint?
  Relation* result = new Relation(ordered_projected_columns, 0);
  for (const vector<int>& t : tuples_)  {
    bool insert_tuple = true;
    for (int i = 0 ; i < t.size(); ++i) {
      if (tuple_index_to_val.count(i) && tuple_index_to_val[i] != t[i]) {
        insert_tuple = false;
        break;
      }
    }
    if (insert_tuple) {
      vector<int> new_tuple(include_column.size());
      for (int i=0; i < include_column.size(); ++i) {
        new_tuple[i] = t[include_column[i]];
      }
      result->AddTuple(new_tuple);
    }
  }
  return result;
}

Relation* Relation::LeftSemiJoin(const vector<int>& tuple, const vector<string>& attrs) const {
  map<int,int> tuple_index_to_val;
  for (int i = 0; i < attrs_.size(); ++i) {
    auto it = find(attrs.begin(), attrs.end(), attrs_[i]);
    if (it != attrs.end()) {
      tuple_index_to_val[i] = tuple[it - attrs.begin()];
    }
  }

  // TODO: improve size hint?
  Relation* result = new Relation(attrs_, 0);
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
  Relation* result = new Relation(concat_attrs, size());

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
  Relation* result = new Relation(attrs_copy, size());

  for (tuple_set::iterator it = tuples_.begin(); it != tuples_.end(); ++it) {
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

ostream& operator<<(ostream& os, const Relation& rel) {
  for (const string& attr : rel.attrs()) {
    cout << attr << "\t";
  }
  cout << endl;

  for (const vector<int>& t : rel.tuples()) {
    for (int i : t) {
      cout << i << "\t";
    }
    cout << endl;
  }

  return os;
}
