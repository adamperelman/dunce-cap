#include "database.h"
#include <algorithm>
#include <unordered_map>
#include <iostream>

using namespace std;

// TODO: replace with better algorithm (e.g. EmptyHeaded)
vector<int> Intersection(const vector<const vector<int>*>& ordered_sets) {
  vector<int> result;
  for (int val : *(ordered_sets[0])) {
    bool is_in_intersection = true;
    for (auto set_it = ordered_sets.begin()+1;
         set_it < ordered_sets.end();
         ++set_it) {
      if (!binary_search((*set_it)->begin(),
                         (*set_it)->end(),
                         val)) {
        is_in_intersection = false;
        break;
      }
    }
    if (is_in_intersection) {
      result.push_back(val);
    }
  }
  return result;
}


// TODO: should we be passing a reference to free_attrs and bound_attrs
// instead of making copies?
TrieNode* GenericJoinInternal(const vector<Relation*>& relations,
                              vector<string> free_attrs,
                              unordered_map<string, int> bound_attrs) {
  if (free_attrs.size() == 1) {
    vector<const vector<int>*> matching_relations;
    for (const Relation* rel : relations) {
      // TODO: only use this relation if it contains a relevant attribute
      if (rel->ContainsAttribute(free_attrs[0])) {
        matching_relations.push_back(&rel->MatchingValues(free_attrs[0], bound_attrs));
      }
    }
    return new TrieNode(Intersection(matching_relations));
  }

  // TODO
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
