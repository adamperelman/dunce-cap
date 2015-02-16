#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "relation.h"
#include <map>
#include <memory>

TrieNode* GenericJoin(std::vector<const TrieNode*>& relations);

#endif /* __DATABASE_H__ */
