#include "relation.h"
#include <iostream>

int main() {
  Relation r("data/simple.txt", "simple", {"a", "b"});
  std::cout << "size: " << r.size() << std::endl;
}
