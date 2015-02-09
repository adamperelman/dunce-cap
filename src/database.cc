#include "database.h"
#include <algorithm>
#include <unordered_map>
#include <iostream>

using namespace std;

bool vectorLengthCmp(const vector<int>* a, const vector<int>* b) {
  return a->size() < b->size();
}

// TODO: replace with better algorithm (e.g. EmptyHeaded)
vector<int> Intersection(vector<const vector<int>*>& ordered_sets) {
  sort(ordered_sets.begin(), ordered_sets.end(), vectorLengthCmp);
  vector<int> intersection(*(ordered_sets.at(0)));
  for (int i = 1; i < ordered_sets.size(); i++) {
    vector<int> temp;
    temp.reserve(intersection.size());
    set_intersection(intersection.begin(), intersection.end(),
                     ordered_sets.at(i)->begin(), ordered_sets.at(i)->end(),
                     inserter(temp, temp.begin()));
    intersection = temp;
  }

  return intersection;
}


// TODO: should we be passing a reference to free_attrs and bound_attrs
// instead of making copies?
TrieNode* GenericJoinInternal(const vector<Relation*>& relations,
                              vector<string>::iterator free_attrs_begin,
                              vector<string>::iterator free_attrs_end,
                              unordered_map<string, int> bound_attrs) {
  if (free_attrs_begin + 1 == free_attrs_end) {
    vector<const vector<int>*> matching_relations;
    for (const Relation* rel : relations) {
      if (rel->ContainsAttribute(*free_attrs_begin)) {
        const vector<int>* vals = rel->MatchingValues(*free_attrs_begin, bound_attrs);
        if (vals) {
          matching_relations.push_back(vals);
        }
      }
    }
    return new TrieNode(*free_attrs_begin, Intersection(matching_relations));
  }

  /* Pick I to be {first attribute}, J = V \ I */
  TrieNode* L = GenericJoinInternal(relations, free_attrs_begin, free_attrs_begin+1, bound_attrs);
  TrieNode* result = new TrieNode(*free_attrs_begin);
  for (int val : L->values()) {
    bound_attrs[*free_attrs_begin] = val;
    TrieNode* righthand_vals = GenericJoinInternal(relations, free_attrs_begin + 1, free_attrs_end, bound_attrs);
    if (righthand_vals->size() > 0) {
      result->AddChildNode(val, righthand_vals); // this does the cartesian product
    }
  }

  delete L;
  return result;
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

  TrieNode* root = GenericJoinInternal(rels_to_join, ordered_attrs.begin(), ordered_attrs.end(), bound_attrs);
  return new Relation(ordered_attrs, root);
}

ostream& operator<<(ostream& os, const Database& db) {
  for (auto rel_it = db.tables_.begin();
       rel_it != db.tables_.end();
       ++rel_it) {
    cout << rel_it->first << endl;
    cout << *(rel_it->second) << endl << endl;
  }

  return os;
}
