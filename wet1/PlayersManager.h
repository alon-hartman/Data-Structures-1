#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include "Player.h"
#include "Group.h"
#include "AVLTree.h"
#include "Level.h"

typedef enum {
    ALLOCATION_ERROR,
    INVALID_INPUT,
    FAILURE,
    SUCCESS
} StatusType;

class PlayersManager {
    private:
        AVL<int, std::shared_ptr<Group>> group_tree;
        AVL<int, std::shared_ptr<Player>> player_tree;
        AVL<int, std::shared_ptr<Level>> level_tree;
        MaxPlayerInfo max_level_player;

        static void AddPlayerToLevelTree(AVL<int, std::shared_ptr<Level>>& level_tree, std::shared_ptr<Player>& player);
        static void AddPlayerToGroupTree(Group& group, std::shared_ptr<Player>& player);
        static void updatePlayersPointers(std::shared_ptr<Group>& group, std::shared_ptr<Node<int, std::shared_ptr<Player>>>& root);
        static void updateGroupPointers(std::shared_ptr<Group>& group, std::shared_ptr<Node<int, std::shared_ptr<Level>>>& root);
        static void updateMaxLevel(AVL<int, std::shared_ptr<Level>>& level_tree, MaxPlayerInfo& max_player_info);

        static Array<Node<int, std::shared_ptr<Level>>*> removeDuplicates(Array<Node<int, std::shared_ptr<Level>>*>& list);
    public:
        // PlayersManager();
        StatusType AddGroup(int groupid);
        StatusType AddPlayer(int playerid, int groupid, int level);
        StatusType RemovePlayer(int playerid);
        StatusType ReplaceGroup(int groupid, int replacementid);
        StatusType IncreaseLevel(int playerid, int levelincrease);
};

#endif