#include "generic_join.h"

#include <iostream>

using namespace std;

Relation* GenericJoin(const vector<Relation>& relations) {
  set<string> attrs;
  for (const Relation& r : relations) {
    attrs.insert(r.attrs().begin(), r.attrs().end());
  }

  if (attrs.size() == 1) {
    return Relation::Intersect(relations);
  }

  return NULL;
}
