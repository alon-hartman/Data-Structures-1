#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include "Player.h"
#include "Group.h"
#include "AVLTree.h"

typedef enum {
    ALLOCATION_ERROR,
    INVALID_INPUT,
    FAILURE,
    SUCCESS
} StatusType;

class PlayersManager {
    private:
        AVL<int, Group> group_tree;
        AVL<Player, Group&> player_tree;
        AVL<Player, Group&> level_tree;
        Player* max_level_player;
    public:
        StatusType AddGroup(int groupid);
        StatusType AddPlayer(int playerid, int groupid, int level);
        StatusType RemovePlayer(int playerid);
        StatusType ReplaceGroup(int groupid, int replacementid);
};

#endif