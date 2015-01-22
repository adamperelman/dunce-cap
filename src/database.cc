#include "database.h"

using namespace std;

Relation* GenericJoinInternal(const vector<Relation>& relations) {
  set<string> attrs;
  for (const Relation& r : relations) {
    attrs.insert(r.attrs().begin(), r.attrs().end());
  }

  if (attrs.size() == 1) {
    return Relation::Intersect(relations);
  }

  return NULL;
}

void Database::AddRelation(Relation relation) {
  tables_.emplace(relation.name(), relation);
}

Relation* Database::GenericJoin(const vector<string>& names) {
  vector<Relation> relations;
  for (const string& name : names) {
    relations.push_back(tables_.at(name));
  }

  return GenericJoinInternal(relations);
}
