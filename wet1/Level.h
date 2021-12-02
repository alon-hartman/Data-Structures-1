#ifndef LEVEL_H
#define LEVEL_H

#include "AVLTree.h"
#include "Player.h"

class Player;
struct Level {
    int id;
    AVL<int, std::shared_ptr<Player>> player_tree;
    Level(const int level) : id(level), player_tree() { }
    Level(const int level, AVL<int, std::shared_ptr<Player>>& player_tree) : id(level), player_tree(player_tree) { }
    bool operator<(const Level& other) {
        return id < other.id;
    }
};

#endif