#include "library1.h"
#include "PlayersManager.h"

void *Init() {
    try {
        PlayersManager *DS = new PlayersManager();
        return (void*)DS;
    }
    catch(const std::bad_alloc& e) {
        return nullptr;
    }
}

StatusType AddGroup(void *DS, int GroupID) {
    if(!DS) {
        return StatusType::INVALID_INPUT;
    }
    return (StatusType)((PlayersManager*)DS)->AddGroup(GroupID);
}

StatusType AddPlayer(void *DS, int PlayerID, int GroupID, int Level) {
    if(!DS) {
        return StatusType::INVALID_INPUT;
    }
    return (StatusType)((PlayersManager*)DS)->AddPlayer(PlayerID, GroupID, Level);
}

StatusType RemovePlayer(void *DS, int PlayerID) {
    if(!DS) {
        return StatusType::INVALID_INPUT;
    }
    return (StatusType)((PlayersManager*)DS)->RemovePlayer(PlayerID);
}

StatusType ReplaceGroup(void *DS, int GroupID, int ReplacementID) {
    if(!DS) {
        return StatusType::INVALID_INPUT;
    }
    return (StatusType)((PlayersManager*)DS)->ReplaceGroup(GroupID, ReplacementID);
}

StatusType IncreaseLevel(void *DS, int PlayerID, int LevelIncrease) {
    if(!DS) {
        return StatusType::INVALID_INPUT;
    }
    return (StatusType)((PlayersManager*)DS)->IncreaseLevel(PlayerID, LevelIncrease);
}

StatusType GetHighestLevel(void *DS, int GroupID, int *PlayerID) {
    if(!DS) {
        return StatusType::INVALID_INPUT;
    }
    return (StatusType)((PlayersManager*)DS)->GetHighestLevel(GroupID, PlayerID);
}

StatusType GetAllPlayersByLevel(void *DS, int GroupID, int **Players, int *numOfPlayers) {
    if(!DS) {
        return StatusType::INVALID_INPUT;
    }
    return (StatusType)((PlayersManager*)DS)->GetAllPlayersByLevel(GroupID, Players, numOfPlayers);
}

StatusType GetGroupsHighestLevel(void *DS, int numOfGroups, int **Players) {
    if(!DS) {
        return StatusType::INVALID_INPUT;
    }
    return (StatusType)((PlayersManager*)DS)->GetGroupsHighestLevel(numOfGroups, Players);
}

void Quit(void** DS) {
    if(!DS) {
        return;
    }
    // ((*(PlayersManager ** )DS))
    // ((*(PlayersManager **)DS))->~PlayersManager();
    delete (PlayersManager*)(*DS);
    *DS = nullptr;
}