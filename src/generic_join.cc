#include "generic_join.h"
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <mutex>
#include <map>
#ifdef _OPENMP
# include <omp.h>
#endif

using namespace std;

bool vectorLengthCmp(const vector<int>* a, const vector<int>* b) {
  return a->size() < b->size();
}

// TODO: replace with better algorithm (e.g. EmptyHeaded)
vector<int>* Intersection(vector<const vector<int>*>& ordered_sets) {
  sort(ordered_sets.begin(), ordered_sets.end(), vectorLengthCmp);
  vector<int>* intersection = new vector<int>(*ordered_sets[0]);
  for (int i = 1; i < ordered_sets.size(); i++) {
    vector<int>* temp = new vector<int>();
    temp->reserve(intersection->size());
    set_intersection(intersection->begin(), intersection->end(),
                     ordered_sets.at(i)->begin(), ordered_sets.at(i)->end(),
                     inserter(*temp, temp->begin()));
    delete intersection;
    intersection = temp;
  }

  return intersection;
}


// TODO: should we be passing a reference to free_attrs and bound_attrs
// instead of making copies?
TrieNode* GenericJoinInternal(vector<const TrieNode*>& relations,
                              vector<string>::iterator free_attrs_begin,
                              vector<string>::iterator free_attrs_end) {
  if (free_attrs_begin + 1 == free_attrs_end) {
    vector<const vector<int>*> matching_relations;
    matching_relations.reserve(relations.size());
    for (const TrieNode* rel : relations) {
      // Note that if this relation contains the given target attribute,
      // it must be the node's own attribute (i.e. not the attribute of one
      // of its children), since we've already bound all previous attributes.
      if (rel->attr() == *free_attrs_begin) {
        matching_relations.push_back(rel->values());
      }
    }
    return new TrieNode(*free_attrs_begin, Intersection(matching_relations));
  }

  /* Pick I to be {first attribute}, J = V \ I */
  const TrieNode* L = GenericJoinInternal(relations, free_attrs_begin, free_attrs_begin+1);
  map<int, TrieNode*> child_nodes;
  mutex child_nodes_lock;
  #pragma omp parallel num_threads(8)
  {
    #pragma omp for
    for (int i = 0; i < L->values()->size(); ++i) {
      int val = L->values()->at(i);
      vector<const TrieNode*> matching_nodes;
      matching_nodes.reserve(relations.size());
      for (const TrieNode* rel : relations) {
        const TrieNode* matching = rel->MatchingNode(*free_attrs_begin, val);
        if (matching) {
          matching_nodes.push_back(matching);
        }
      }
      TrieNode* righthand_vals = GenericJoinInternal(matching_nodes, free_attrs_begin + 1, free_attrs_end);
      if (righthand_vals->size() > 0) {
        lock_guard<mutex> lock(child_nodes_lock);
        child_nodes[val] = righthand_vals; // this does the cartesian product
        #pragma omp flush(child_nodes)
      } else {
        delete righthand_vals;
      }
    }
  }

  TrieNode* result = new TrieNode(*free_attrs_begin);
  for (pair<int, TrieNode*> val_child : child_nodes) {
    result->AddChildNode(val_child.first, val_child.second);
  }

  delete L;
  return result;
}


TrieNode* GenericJoin(vector<const TrieNode*>& relations) {

  set<string> attrs;
  for (const TrieNode* r : relations) {
    for (const string& attr : r->attrs()) {
      attrs.insert(attr);
    }
  }

  vector<string> ordered_attrs(attrs.begin(), attrs.end());

  return GenericJoinInternal(relations, ordered_attrs.begin(), ordered_attrs.end());
}

