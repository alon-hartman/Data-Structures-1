#ifndef GROUP_H
#define GROUP_H

#include "AVLTree.h"
#include "Level.h"

struct MaxPlayerInfo {
    int id;
    int level;
    MaxPlayerInfo() : id(-1), level(-1) {}
};

class Player;
struct Group {
    int id;
    int size;
    AVL<int, std::shared_ptr<Level>> level_tree;
    MaxPlayerInfo max_level_player;
    
    Group(const int id) : id(id), size(0), level_tree(), max_level_player() { }

    Group(int id, AVL<int, std::shared_ptr<Level>>& level_tree) : id(id), size(0), level_tree(level_tree), max_level_player() { }
};

#endif