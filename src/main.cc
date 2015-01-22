#include "relation.h"
#include <iostream>

using namespace std;

int main() {
  Relation r("data/simple.txt", "simple", {"a", "b"});
  cout << r;
}
