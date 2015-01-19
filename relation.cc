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
    tuples_.push_back(tuple);
  } 
}

Relation* Relation::Project(vector<string> attrs) {
  return NULL;
}

Relation* Relation::LeftSemiJoin(Relation relation) {
  return NULL;
}
