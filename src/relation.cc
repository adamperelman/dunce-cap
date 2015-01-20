#include "relation.h"
#include <sstream>
#include <fstream>
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

void Relation::AddTuple(vector<int> tuple) {
  tuples_.insert(tuple);
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

Relation* Relation::LeftSemiJoin(Relation relation) const {
  return NULL;
}

int Relation::size() const {
  return tuples_.size();
}

bool Relation::contains(const std::vector<int>& tuple) const {
  return false;
}
