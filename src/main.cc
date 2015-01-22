#include "relation.h"
#include <iostream>

#include "relation.h"
#include "database.h"

using namespace std;

int main() {
  Database db;
  db.AddRelation(new Relation("data/facebook_combined.txt", "R", {"a", "b"}));
  db.AddRelation(new Relation("data/facebook_combined.txt", "S", {"b", "c"}));
  db.AddRelation(new Relation("data/facebook_combined.txt", "T", {"c", "a"}));

  Relation* result = db.GenericJoin({"R", "S", "T"});

  cout << "size: " << result->size() << endl;
}
