#include "relation.h"
#include <iostream>
#include <chrono>

#include "relation.h"
#include "database.h"

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    cout << "Usage: ./triangle.exe <filename>" << endl;
    exit(0);
  }


  Database db;

  cout << "loading database..." << endl;
  db.AddRelation(new Relation(argv[1], "R", {"a", "b"}));
  db.AddRelation(new Relation(argv[1], "S", {"b", "c"}));
  db.AddRelation(new Relation(argv[1], "T", {"c", "a"}));

  cout << "loaded database" << endl;

  cout << "performing join..." << endl;
  typedef chrono::high_resolution_clock Clock;
  typedef chrono::milliseconds ms;

  Clock::time_point start_join = Clock::now();
  Relation* result = db.GenericJoin({"R", "S", "T"});
  Clock::time_point end_join = Clock::now();

  cout << "done performing join" << endl;
  ms join_time = chrono::duration_cast<ms>(end_join - start_join);

  cout << join_time.count() << "ms\n" << endl;
  cout << "size: " << result->size() << endl;

  delete result;
}
