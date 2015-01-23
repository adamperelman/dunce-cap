#include "relation.h"
#include <iostream>

#include "relation.h"
#include "database.h"

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    cout << "Usage: ./triangle.exe <filename>" << endl;
    exit(0);
  }


  Database db;
  db.AddRelation(new Relation(argv[1], "R", {"a", "b"}));
  db.AddRelation(new Relation(argv[1], "S", {"b", "c"}));
  db.AddRelation(new Relation(argv[1], "T", {"c", "a"}));

  Relation* result = db.GenericJoin({"R", "S", "T"});

  cout << "size: " << result->size() << endl;
}
