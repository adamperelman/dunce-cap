#include "database.h"
#include <iostream>

using namespace std;

vector<Relation*> ProjectOverlapping(
  const set<string>& attributes,
  const vector<Relation*>& relations) {
  vector<Relation*> results;
  for (const Relation* rel : relations) {
    if (rel->ContainsAttributes(attributes)) {
      results.push_back(rel->Project(attributes));
    }
  }
  return results;
}

vector<Relation*> SemiJoinThenProjectOverlapping(
  const set<string>& attributes,
  const vector<Relation*>& relations,
  const vector<int>& tuple_to_semijoin,
  const vector<string>& attrs_to_semijoin) {
  vector<Relation*> results;
  for (const Relation* rel : relations) {
    if (rel->ContainsAttributes(attributes)) {
      unique_ptr<Relation> joined_rel(rel->LeftSemiJoin(tuple_to_semijoin, attrs_to_semijoin));
      results.push_back(joined_rel->Project(attributes));
    }
  }
  return results;
}

Relation* GenericJoinInternal(const vector<Relation*>& relations) {
  set<string> attrs;
  for (const Relation* r : relations) {
    attrs.insert(r->attrs().begin(), r->attrs().end());
  }

  // for (string a : attrs) {
  //  cout << a;
  // }
  // cout << endl;

  if (attrs.size() == 1) {
    return Relation::Intersect(relations);
  }

  /* Pick I to be {first attribute}, J = V \ I */
  auto it = attrs.begin();
  set<string> I;
  I.insert(*it);
  ++it;
  set<string> J(it, attrs.end());

  unique_ptr<Relation> L(GenericJoinInternal(ProjectOverlapping(I, relations)));

  vector<unique_ptr<Relation>> partial_results;
  for (const vector<int>& t : L->tuples()) {
    unique_ptr<Relation> result_per_tuple(GenericJoinInternal(SemiJoinThenProjectOverlapping(J, relations, t, L->attrs())));
    unique_ptr<Relation> product(result_per_tuple->CartesianProduct(t, L->attrs()));
    if (product->size()) {
      partial_results.push_back(move(product));
    }
  }

  return Relation::Union(partial_results);
}



void Database::AddRelation(Relation* relation) {
  tables_.emplace(relation->name(), unique_ptr<Relation>(relation));
}

Relation* Database::GenericJoin(const vector<string>& names) {
  vector<Relation*> relations;
  for (const string& name : names) {
    relations.push_back(tables_.at(name).get());
  }

  return GenericJoinInternal(relations);
}
