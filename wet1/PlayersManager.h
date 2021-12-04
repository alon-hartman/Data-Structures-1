#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include "Player.h"
#include "Group.h"
#include "AVLTree.h"
#include "Level.h"

typedef enum {
    PM_ALLOCATION_ERROR = -2,
    PM_INVALID_INPUT = -3,
    PM_FAILURE = -1,
    PM_SUCCESS = 0
} PMStatusType;

class PlayersManager {
    private:
        AVL<int, std::shared_ptr<Group>> group_tree;
        AVL<int, std::shared_ptr<Group>> not_empty_group_tree;
        AVL<int, std::shared_ptr<Player>> player_tree;
        AVL<int, std::shared_ptr<Level>> level_tree;
        MaxPlayerInfo max_level_player;

        static void AddPlayerToLevelTree(AVL<int, std::shared_ptr<Level>>& level_tree, std::shared_ptr<Player>& player);
        static void AddPlayerToGroupTree(Group& group, std::shared_ptr<Player>& player);
        static void updatePlayersPointers(std::shared_ptr<Group>& group, std::shared_ptr<Node<int, std::shared_ptr<Player>>>& root);
        static void updateGroupPointers(std::shared_ptr<Group>& group, std::shared_ptr<Node<int, std::shared_ptr<Level>>>& root);
        static void updateMaxLevel(AVL<int, std::shared_ptr<Level>>& level_tree, MaxPlayerInfo& max_player_info);

        static Array<Node<int, std::shared_ptr<Level>>*> removeDuplicates(Array<Node<int, std::shared_ptr<Level>>*>& list);

        static void ReverseInorderLevelTree(Array<int>& Players, std::shared_ptr<Node<int, std::shared_ptr<Level>>>& root);
        static void InorderPlayerTree(Array<int>& Players, std::shared_ptr<Node<int, std::shared_ptr<Player>>>& root);
        static void InorderGroupTree(Array<int>& array, std::shared_ptr<Node<int, std::shared_ptr<Group>>>& root, int* printed);

        // static int* GetAllPlayersByLevelAux(int num_of_players, AVL<int, std::shared_ptr<Level>>& level_tree);
    public:
        // PlayersManager();
        PMStatusType AddGroup(int groupid);
        PMStatusType AddPlayer(int playerid, int groupid, int level);
        PMStatusType RemovePlayer(int playerid);
        PMStatusType ReplaceGroup(int groupid, int replacementid);
        PMStatusType IncreaseLevel(int playerid, int levelincrease);
        PMStatusType GetHighestLevel(int groupid, int* playerid);
        PMStatusType GetAllPlayersByLevel(int groupid, int **Players, int *numOfPlayers);
        PMStatusType GetGroupsHighestLevel(int numOfGroups, int** Players);
};

#endif