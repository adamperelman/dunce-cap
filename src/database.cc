#include "database.h"

using namespace std;

vector<Relation*> ProjectOverlapping(
  const set<string>& attributes, 
  const vector<Relation*>& relations, 
  const vector<int>& tuple_to_semijoin = vector<int>(),
  const vector<string>& attrs_to_semijoin = vector<string>()) {
  vector<Relation*> results; // TODO: change this stuff to pointers
  for (const Relation* rel : relations) {
    if (rel->ContainsAttributes(attributes)) {
      if (!tuple_to_semijoin.empty()) {
        rel = rel->LeftSemiJoin(tuple_to_semijoin, attrs_to_semijoin);
      }
      results.push_back(rel->Project(attributes));
    }
  }
  return results;
}

Relation* GenericJoinInternal(const vector<Relation*>& relations) {
  /* Compute script V */
  set<string> attrs;
  for (const Relation* r : relations) {
    attrs.insert(r->attrs().begin(), r->attrs().end());
  }

  if (attrs.size() == 1) {
    return Relation::Intersect(relations);
  }

  /* Pick I to be {first attribute}, J = V \ I */
  auto it = attrs.begin();
  set<string> I;
  I.insert(*it);
  ++it;
  set<string> J(it, attrs.end()); 

  Relation* L = GenericJoinInternal(ProjectOverlapping(attrs, relations)); 
  
  for (const vector<int>& t : L->tuples()) {
    
  } 

  return NULL;
}



void Database::AddRelation(Relation* relation) {
  tables_.emplace(relation->name(), relation);
}

Relation* Database::GenericJoin(const vector<string>& names) {
  vector<Relation*> relations;
  for (const string& name : names) {
    relations.push_back(tables_.at(name));
  }

  return GenericJoinInternal(relations);
}
