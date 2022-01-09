#include "PlayersManager.h"
#include "Player.h"
#include <memory>
#include <exception>
#include <iostream>

PlayersManager::PlayersManager(int k, int scale) : groups(k, scale+1), all_players_tree(scale+1), all_players(), scale(scale) { }  // O(k)

PMStatusType PlayersManager::mergeGroups(int GroupID1, int GroupID2) {
    if(GroupID1 <= 0 || GroupID2 <= 0 || GroupID1 > groups.Size() || GroupID2 > groups.Size()) {
        return PM_INVALID_INPUT;
    }
    int root1 = groups.Find(GroupID1-1);  // O(log*k) amortized with Union
    int root2 = groups.Find(GroupID2-1);  // O(log*k) amortized with Union
    if(root1 != root2) {
        groups.Union(root1, root2);  // O(log*k + n) amortized with Find
    }
    return PM_SUCCESS;
}

PMStatusType PlayersManager::addPlayer(int PlayerID, int GroupID, int score) {
    if(GroupID <= 0 || GroupID > groups.Size() || score <= 0 || score > scale || PlayerID <= 0) {
        return PM_INVALID_INPUT;
    }
    if(all_players.findPlayer(PlayerID) != nullptr) {  // O(1) amortized with average input
        return PM_FAILURE;
    }
    try {
        std::shared_ptr<Player> player = std::make_shared<Player>(PlayerID, GroupID, score, 0);
        all_players_tree.insert(player);  // O(1)
        all_players.addPlayer(player);  // O(1) amortized with average input
        int root = groups.Find(GroupID-1);  // O(log*k) amortized with Union
        groups.groups[root].rt->insert(player);  // O(1)
        groups.groups[root].number_of_players++;
    }
    catch(std::bad_alloc&) {
        return PM_ALLOCATION_ERROR;
    }
    return PM_SUCCESS;
}

PMStatusType PlayersManager::removePlayer(int PlayerID) {
    if(PlayerID <= 0) {
        return PM_INVALID_INPUT;
    }
    std::shared_ptr<Player> player = all_players.findPlayer(PlayerID);
    if(player == nullptr) {  // O(1) amortized with average input
        return PM_FAILURE;
    }
    try {
        all_players.removePlayer(player);  // O(1) amortized with average input
        all_players_tree.removePlayer(player);  // O(logn)
        int root = groups.Find(player->groupID-1);  // O(log*k) amortized with Union
        groups.groups[root].rt->removePlayer(player);  // O(logn)
        groups.groups[root].number_of_players--;
    }
    catch(std::bad_alloc&) {
        return PM_ALLOCATION_ERROR;
    }
    return PM_SUCCESS;
}

PMStatusType PlayersManager::increasePlayerIDLevel(int PlayerID, int LevelIncrease){
    if(PlayerID <= 0 || LevelIncrease <= 0) {
        return PM_INVALID_INPUT;
    }
    std::shared_ptr<Player> player = all_players.findPlayer(PlayerID);
    if(player == nullptr) {  // O(1) amortized with average input
        return PM_FAILURE;
    }
    try {
        all_players_tree.removePlayer(player);  // O(1) if level = 0, O(logn) otherwise
        int root = groups.Find(player->groupID-1);  // O(log*k) amortized with Union
        groups.groups[root].rt->removePlayer(player);  // O(1) if level = 0, O(logn) otherwise
        groups.groups[root].number_of_players--;
        player->level += LevelIncrease;
        all_players_tree.insert(player); // O(1) if level = 0, O(logn) otherwise
        groups.groups[root].rt->insert(player); // O(1) if level = 0, O(logn) otherwise
        groups.groups[root].number_of_players++;
    }
    catch(std::bad_alloc&) {
        return PM_ALLOCATION_ERROR;
    }
    return PM_SUCCESS;
}


PMStatusType PlayersManager::changePlayerIDScore(int PlayerID, int NewScore) {
    if(PlayerID <= 0 || NewScore <= 0 || NewScore > scale) {
        return PM_INVALID_INPUT;
    }
    std::shared_ptr<Player> player = all_players.findPlayer(PlayerID);
    if(player == nullptr) {  // O(1) amortized with average input
        return PM_FAILURE;
    }
    try {
        all_players_tree.change_player_score(player->level, player->score, NewScore);
        int root = groups.Find(player->groupID-1);  // O(log*k) amortized with Union
        groups.groups[root].rt->change_player_score(player->level, player->score, NewScore);

        player->score = NewScore;
    }
    catch(std::bad_alloc&) {
        return PM_ALLOCATION_ERROR;
    }
    return PM_SUCCESS;
}

PMStatusType PlayersManager::getPercentOfPlayersWithScoreInBounds(int GroupID, int score, int lowerlevel, int higherlevel, double* players) {
    try {
        if(GroupID < 0 || GroupID > groups.Size() || !players) {
            return PM_INVALID_INPUT;
        }
        double res;
        if(GroupID == 0) {
            res = all_players_tree.getPercentOfPlayersWithScoreInBounds(lowerlevel, higherlevel, score);
        }
        else {
            int root = groups.Find(GroupID-1);
            res = groups.groups[root].rt->getPercentOfPlayersWithScoreInBounds(lowerlevel, higherlevel, score);
        }
        if(res == -1) {  // no players in lowerlevel, upperlevel
            return PM_FAILURE;
        }
        *players = res;
    }
    catch(std::bad_alloc&) {
        return PM_ALLOCATION_ERROR;
    }
    return PM_SUCCESS;
}

PMStatusType PlayersManager::averageHighestPlayerLevelByGroup(int GroupID, int m, double* avgLevel) {
    try {
        if(GroupID < 0 || GroupID > groups.Size() || !avgLevel || m <= 0) {
            return PM_INVALID_INPUT;
        }
        if(GroupID == 0) {
            if(m > all_players_tree.getPlayersInTree()) {  // O(logn)
                return PM_FAILURE;
            }
            *avgLevel = all_players_tree.averageHighestPlayerLevelByGroup(m);  // O(logn)
        }
        else {
            int root = groups.Find(GroupID-1);
            if(m > groups.groups[root].rt->getPlayersInTree()) {  // O(logn)
                return PM_FAILURE;
            }
            *avgLevel = groups.groups[root].rt->averageHighestPlayerLevelByGroup(m);  // O(logn)
        }
    }
    catch(std::bad_alloc&) {
        return PM_ALLOCATION_ERROR;
    }
    return PM_SUCCESS;
}

PMStatusType PlayersManager::getPlayersBound(int GroupID, int score, int m, int * LowerBoundPlayers, int * HigherBoundPlayers) {
    if(!LowerBoundPlayers || !HigherBoundPlayers || GroupID > groups.Size() || GroupID < 0 || m < 0 || score <= 0 || score > scale) {
        return PM_INVALID_INPUT;
    }
    if(GroupID == 0) {
        if(m > all_players_tree.getPlayersInTree()) {  // O(logn)
            return PM_FAILURE;
        }
        all_players_tree.getPlayersBounds(score, m, LowerBoundPlayers, HigherBoundPlayers);
    }
    else {
        int root = groups.Find(GroupID-1);
        if(m > groups.groups[root].rt->getPlayersInTree()) {  // O(logn)
            return PM_FAILURE;
        }
        groups.groups[root].rt->getPlayersBounds(score, m, LowerBoundPlayers, HigherBoundPlayers);
    }
    return PM_SUCCESS;
}