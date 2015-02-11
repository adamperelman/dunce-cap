#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "relation.h"
#include <map>
#include <memory>

class Database {
public:
  Database() {}
  ~Database() {}

  void AddRelation(const std::string& relation_name, TrieNode* relation);
  TrieNode* GenericJoin(const std::vector<std::string>& names) const;

private:
  std::map<std::string, std::unique_ptr<TrieNode>> tables_;

  friend std::ostream& operator<<(std::ostream& os, const Database& db);
};
#endif /* __DATABASE_H__ */
