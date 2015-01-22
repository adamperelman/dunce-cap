#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "relation.h"
#include <map>

class Database {
public:
  Database() {}
  ~Database() {}

  void AddRelation(Relation relation);
  Relation* GenericJoin(const std::vector<std::string>& names);

private:
  std::map<std::string, Relation> tables_;
};
#endif /* __DATABASE_H__ */
