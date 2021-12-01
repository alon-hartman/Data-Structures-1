#include "PlayersManager.h"
#include <cassert>

StatusType PlayersManager::AddGroup(int groupid) {
    if(groupid <= 0) {
        return INVALID_INPUT;
    }
    try {
        Group group(groupid);
        group_tree.insert(groupid, group);
    }
    catch(const std::bad_alloc& e) {
        return ALLOCATION_ERROR;
    }
    catch(const KeyAlreadyExists& e) {
        return FAILURE;
    }
    return SUCCESS;
}
/**
 * TODO: add max player maintaince.
 */
StatusType PlayersManager::AddPlayer(int playerid, int groupid, int level) {
    if(playerid <= 0 || groupid <= 0 || level < 0) {
        return INVALID_INPUT;
    }
    try {
        if(group_tree.find(groupid) == nullptr) {  // O(logk)
            return FAILURE;
        }
        Group* group = &((group_tree.find(groupid))->info);  // O(logk)
        Player player(playerid, level);
        player_tree.insert(player, *group);  // O(logn)
        player.changeMethod();
        level_tree.insert(player, *group);  // O(logn)
        group->level_tree.insert(player, *group); // O(logn)
    }
    catch(const std::bad_alloc& e) {
        return ALLOCATION_ERROR;
    }
    catch(const KeyAlreadyExists& e) {
        return FAILURE;
    }
    return SUCCESS;
}

StatusType PlayersManager::RemovePlayer(int playerid) { // O(logn)
    if(playerid <= 0) {
        return INVALID_INPUT;
    }
    try {
        Player player(playerid);
        if(player_tree.find(player) == nullptr) {  // O(logn)
            return FAILURE;
        }
        Group& group = (player_tree.find(player)->info);  // O(logn)
        player = player_tree.find(player)->key;  // O(logn)
        group.level_tree.remove(player);  // O(logn)
        level_tree.remove(player);  // O(logn)
        player_tree.remove(player);  // O(logn)
    }
    catch(const KeyDoesNotExist& e) {
        return FAILURE;
    }
    return SUCCESS;
}

StatusType PlayersManager::ReplaceGroup(int groupid, int replacementid) {
    if(groupid <= 0 || replacementid <= 0 || groupid == replacementid) {
        return FAILURE;
    }
    try {
        if(group_tree.find(groupid) == nullptr || group_tree.find(replacementid) == nullptr) {  // O(logk)
            return FAILURE;
        }
        Group* g1 = &(group_tree.find(groupid)->info);  // O(logk)
        Group* g2 = &(group_tree.find(replacementid)->info); // O(logk)
        AVL<Player, Group&> merged = AVL<Player, Group&>::merge(g1->level_tree, g2->level_tree);  // O(m+n)
        // Group temp1 = *g1;
        // Group temp2 = *g2;
        group_tree.remove(groupid);  // O(logk + m)
        group_tree.remove(replacementid); // O(logk + n)
        group_tree.find(replacementid);
        group_tree.insert(replacementid, Group(replacementid, merged)); // O(logk + m + n)
        // Group* temp = g2;
        g2 = &(group_tree.find(replacementid)->info);
        // delete temp;
        // temp = g1;
        g1 = &(group_tree.find(replacementid)->info);
        // delete temp;
        // delete new_group;
    }
    catch(const std::bad_alloc& e) {
        return ALLOCATION_ERROR;
    }
    return SUCCESS;
}

int main() {
    PlayersManager pm;
    assert(pm.AddGroup(2) == SUCCESS);
    assert(pm.AddGroup(3) == SUCCESS);
    assert(pm.AddGroup(5) == SUCCESS);
    assert(pm.AddGroup(1) == SUCCESS);
    assert(pm.AddGroup(7) == SUCCESS);
    assert(pm.AddGroup(2) == FAILURE);
    assert(pm.AddPlayer(1, 2, 1) == SUCCESS);
    assert(pm.AddPlayer(2, 2, 1) == SUCCESS);
    assert(pm.AddPlayer(3, 2, 1) == SUCCESS);
    assert(pm.AddPlayer(4, 3, 1) == SUCCESS);
    assert(pm.AddPlayer(5, 5, 1) == SUCCESS);
    assert(pm.AddPlayer(1, 2 ,1) == FAILURE);
    assert(pm.AddPlayer(1, 4 ,1) == FAILURE);
    assert(pm.AddPlayer(1, 0 ,1) == INVALID_INPUT);
    assert(pm.RemovePlayer(1) == SUCCESS);
    assert(pm.RemovePlayer(1) == FAILURE);
    assert(pm.AddPlayer(1, 2, 1) == SUCCESS);
    // pm.AddPlayer(1, 2, 1);
    assert(pm.ReplaceGroup(2, 3) == SUCCESS);
    assert(pm.AddGroup(2) == SUCCESS);
    assert(pm.AddGroup(3) == FAILURE);
    assert(pm.AddPlayer(1, 2, 1) == FAILURE);
    assert(pm.AddPlayer(4, 2, 1) == FAILURE);
    assert(pm.AddPlayer(1, 4, 1) == FAILURE);
    assert(pm.AddPlayer(5, 5, 1) == FAILURE);
}