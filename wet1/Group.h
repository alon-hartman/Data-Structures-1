#ifndef GROUP_H
#define GROUP_H

#include "AVLTree.h"
// #include "Player.h"
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
    
    Group(const int id) : id(id), level_tree(), max_level_player(), size(0) { }

    Group(int id, AVL<int, std::shared_ptr<Level>>& level_tree) : id(id), level_tree(level_tree), max_level_player(), size(0) { }
    // Group& operator=(const Group& other) {
    //     if(this == &other) {
    //         return *this;
    //     }
    //     id = other.id;
    //     level_tree.clear();
    //     level_tree.copyRoot(other.level_tree);
    //     max_level_player = other.max_level_player;
    //     return *this;
    // }
};

#endif