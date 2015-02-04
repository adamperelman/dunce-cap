#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "relation.h"
#include <map>
#include <memory>

class Database {
public:
  Database() {}
  ~Database() {}

  void AddRelation(Relation* relation);
  Relation* GenericJoin(const std::vector<std::string>& names) const;

private:
  std::map<std::string, std::unique_ptr<Relation>> tables_;

  friend std::ostream& operator<<(std::ostream& os, const Database& db);
};
#endif /* __DATABASE_H__ */
