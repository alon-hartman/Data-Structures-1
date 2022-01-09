#ifndef PLAYERS_MANAGE_H
#define PLAYERS_MANAGER_H

#include "UF.h"
#include "RankTree.h"
#include "DynamicHashTable.h"

typedef enum {
    PM_SUCCESS = 0,
    PM_FAILURE = -1,
    PM_ALLOCATION_ERROR = -2,
    PM_INVALID_INPUT = -3
} PMStatusType;

class PlayersManager {
    private:
        UF groups;
        RankTree all_players_tree;
        DHT all_players;
        int scale;

    public:
        PlayersManager(int k, int scale);
        PMStatusType mergeGroups(int GroupID1, int GroupID2);
        PMStatusType addPlayer(int PlayerID, int GroupID, int score);
        PMStatusType removePlayer(int PlayerID);
        PMStatusType increasePlayerIDLevel(int PlayerID, int LevelIncrease);
        PMStatusType changePlayerIDScore(int PlayerID, int NewScore);
        PMStatusType getPercentOfPlayersWithScoreInBounds(int GroupID, int score, int lowerlevel, int higherlevel, double* players);
        PMStatusType averageHighestPlayerLevelByGroup(int GroupID, int m, double* avgLevel);
        PMStatusType getPlayersBound(int GroupID, int score, int m, int * LowerBoundPlayers, int * HigherBoundPlayers);
        // ~PlayersManager();
        

};

#endif