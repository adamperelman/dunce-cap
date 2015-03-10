#include "generic_join.h"
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <mutex>
#include <map>

using namespace std;

// TODO: make sure we use these global vars safely
// if we do multithreading!
vector<int> scratch_space[2];

bool vectorLengthCmp(const vector<int>* a, const vector<int>* b) {
  return a->size() < b->size();
}

inline vector<int> Intersection(vector<const vector<int>*>& ordered_sets) {
  sort(ordered_sets.begin(), ordered_sets.end(), vectorLengthCmp);

  const vector<int>* prev_result = ordered_sets[0];
  vector<int>* new_result = &scratch_space[0];

  for (int i = 1; i < ordered_sets.size(); i++) {
    new_result->clear();
    set_intersection(prev_result->begin(), prev_result->end(),
                     ordered_sets[i]->begin(), ordered_sets[i]->end(),
                     inserter(*new_result, new_result->begin()));

    prev_result = new_result;
    new_result = &scratch_space[i % 2];
  }

  return *prev_result;
}

inline vector<const vector<int>*> MatchingNodesForAttr(vector<const TrieNode*>& relations,
                                                       const string& attr) {
  vector<const vector<int>*> matching_relations;
  matching_relations.reserve(relations.size());
  for (const TrieNode* rel : relations) {
    // Note that if this relation contains the given target attribute,
    // it must be the node's own attribute (i.e. not the attribute of one
    // of its children), since we've already bound all previous attributes.
    if (rel->attr() == attr) {
      matching_relations.push_back(&rel->values());
    }
  }
  return matching_relations;
}

inline vector<const TrieNode*> MatchingNodesForVal(vector<const TrieNode*>& relations,
                                                   const string& attr,
                                                   int val) {
  vector<const TrieNode*> matching_nodes;
  matching_nodes.reserve(relations.size());
  for (const TrieNode* rel : relations) {
    const TrieNode* matching = rel->MatchingNode(attr, val);
    if (matching) {
      matching_nodes.push_back(matching);
    }
  }
  return matching_nodes;
}

// TODO: if we know two relations come from the same dataset,
// we can skip the intersection, since we know they'll be identical
TrieNode* GenericJoinInternal(vector<const TrieNode*>& relations,
                              vector<string>::iterator free_attrs_begin,
                              vector<string>::iterator free_attrs_end) {
  if (free_attrs_begin + 1 == free_attrs_end) {
    vector<const vector<int>*> matching_relations = MatchingNodesForAttr(relations,
                                                                             *free_attrs_begin);
    return new TrieNode(*free_attrs_begin, Intersection(matching_relations));
  }

  /* Pick I to be {first attribute}, J = V \ I */
  TrieNode* L = GenericJoinInternal(relations, free_attrs_begin, free_attrs_begin+1);
  TrieNode* result = new TrieNode(*free_attrs_begin);
  for (int val : L->values()) {
    vector<const TrieNode*> matching_nodes = MatchingNodesForVal(relations,
                                                                 *free_attrs_begin,
                                                                 val);

    TrieNode* righthand_vals = GenericJoinInternal(matching_nodes, free_attrs_begin + 1, free_attrs_end);
    if (righthand_vals->size() > 0) {
      result->AddChildNode(val, righthand_vals);
    } else {
      delete righthand_vals;
    }
  }

  delete L;
  return result;
}


int GenericJoinCountInternal(vector<const TrieNode*>& relations,
                             vector<string>::iterator free_attrs_begin,
                             vector<string>::iterator free_attrs_end) {
  if (free_attrs_begin + 1 == free_attrs_end) {
    vector<const vector<int>*> matching_relations = MatchingNodesForAttr(relations,
                                                                         *free_attrs_begin);
    return Intersection(matching_relations).size();
  }

  /* Pick I to be {first attribute}, J = V \ I */
  TrieNode* L = GenericJoinInternal(relations, free_attrs_begin, free_attrs_begin+1);
  int count = 0;
  for (int val : L->values()) {
    vector<const TrieNode*> matching_nodes = MatchingNodesForVal(relations,
                                                                 *free_attrs_begin,
                                                                 val);

    count += GenericJoinCountInternal(matching_nodes, free_attrs_begin + 1, free_attrs_end);
  }

  delete L;
  return count;
}


vector<string> OrderedAttrs(vector<const TrieNode*>& relations) {
  set<string> attrs;
  for (const TrieNode* r : relations) {
    for (const string& attr : r->attrs()) {
      attrs.insert(attr);
    }
  }

  vector<string> ordered_attrs(attrs.begin(), attrs.end());
  return ordered_attrs;
}

void InitializeScratchSpace(vector<const TrieNode*>& relations) {
  scratch_space[0] = vector<int>();
  scratch_space[1] = vector<int>();
}

TrieNode* GenericJoin(vector<const TrieNode*>& relations) {
  InitializeScratchSpace(relations);
  vector<string> ordered_attrs = OrderedAttrs(relations);
  return GenericJoinInternal(relations, ordered_attrs.begin(), ordered_attrs.end());
}

int GenericJoinCount(vector<const TrieNode*>& relations) {
  InitializeScratchSpace(relations);
  vector<string> ordered_attrs = OrderedAttrs(relations);
  return GenericJoinCountInternal(relations, ordered_attrs.begin(), ordered_attrs.end());
}
