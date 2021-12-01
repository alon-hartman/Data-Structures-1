#ifndef GROUP_H
#define GROUP_H

#include "AVLTree.h"
#include "Player.h"

struct Group {
    int id;
    AVL<Player, Group&> level_tree;
    Player* max_level_player;
    
    Group(int id) {
        this->id = id;
        AVL<Player, Group&> level_tree;
        max_level_player = nullptr;
    }
    Group(int id, AVL<Player,Group&>& level_tree){
        this->id = id;
        this->level_tree = level_tree;
        max_level_player = nullptr;// not correct
    }
    
};

#endif