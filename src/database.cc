#include "database.h"
#include <unordered_map>
#include <iostream>

using namespace std;


TrieNode* GenericJoinInternal(const vector<Relation*>& relations,
                              vector<string> free_attrs,
                              unordered_map<string, int> bound_attrs) {
  return NULL;
}

void Database::AddRelation(Relation* relation) {
  tables_.emplace(relation->name(), unique_ptr<Relation>(relation));
}

Relation* Database::GenericJoin(const vector<string>& names) const {
  vector<Relation*> rels_to_join;

  set<string> attrs;
  for (const string& name : names) {
    Relation* r = tables_.at(name).get();
    rels_to_join.push_back(r);
    attrs.insert(r->attrs().begin(), r->attrs().end());
  }

  vector<string> ordered_attrs(attrs.begin(), attrs.end());
  unordered_map<string, int> bound_attrs; // Nothing is bound yet.

  TrieNode* root = GenericJoinInternal(rels_to_join, ordered_attrs, bound_attrs);
  return new Relation(ordered_attrs, root);
}
