#include "library2.h"
#include "PlayersManager.h"

void* Init(int k, int scale) {
    if(k <=0 || scale > 200 || scale <= 0) {
        return nullptr;
    }
    PlayersManager *DS = new PlayersManager(k, scale);
    return (void*)DS;
}

StatusType MergeGroups(void *DS, int GroupID1, int GroupID2) {
    if(!DS) {
        return StatusType::INVALID_INPUT;
    }
    return (StatusType)((PlayersManager*)DS)->mergeGroups(GroupID1, GroupID2);
}

StatusType AddPlayer(void* DS, int PlayerID, int GroupID, int score) {
    if(!DS) {
        return StatusType::INVALID_INPUT;
    }
    return (StatusType)((PlayersManager*)DS)->addPlayer(PlayerID, GroupID, score);
}

StatusType RemovePlayer(void* DS, int PlayerID) {
    if(!DS) {
        return StatusType::INVALID_INPUT;
    }
    return (StatusType)((PlayersManager*)DS)->removePlayer(PlayerID);
}

StatusType IncreasePlayerIDLevel(void* DS, int PlayerID, int LevelIncrease) {
    if(!DS) {
        return StatusType::INVALID_INPUT;
    }
    return (StatusType)((PlayersManager*)DS)->increasePlayerIDLevel(PlayerID, LevelIncrease);
}

StatusType ChangePlayerIDScore(void* DS, int PlayerID, int NewScore) {
    if(!DS) {
        return StatusType::INVALID_INPUT;
    }
    return (StatusType)((PlayersManager*)DS)->changePlayerIDScore(PlayerID, NewScore);
}

StatusType GetPercentOfPlayersWithScoreInBounds(void* DS, int GroupID, int score, int lowerlevel, int higherlevel, double* players) {
    if(!DS) {
        return StatusType::INVALID_INPUT;
    }
    return (StatusType)((PlayersManager*)DS)->getPercentOfPlayersWithScoreInBounds(GroupID, score, lowerlevel, higherlevel, players);
}

StatusType AverageHighestPlayerLevelByGroup(void* DS, int GroupID, int m, double* avgLevel) {
    if(!DS) {
        return StatusType::INVALID_INPUT;
    }
    return (StatusType)((PlayersManager*)DS)->averageHighestPlayerLevelByGroup(GroupID, m, avgLevel);
}

StatusType GetPlayersBound(void *DS, int GroupID, int score, int m, int * LowerBoundPlayers, int * HigherBoundPlayers) {
    return SUCCESS;
}

void Quit(void** DS) {
    if(!DS) {
        return;
    }
    delete (PlayersManager*)(*DS);
    *DS = nullptr;
}
