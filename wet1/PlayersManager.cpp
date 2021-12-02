#include "PlayersManager.h"
#include <cassert>

/** *******************************************************************************************************************
 *                                      STATIC HELPER FUNCTIONS                                                       *
***********************************************************************************************************************/

void PlayersManager::AddPlayerToLevelTree(AVL<int, std::shared_ptr<Level>>& level_tree, std::shared_ptr<Player>& player) {
    if(!level_tree.find(player->level)) {
        std::shared_ptr<Level> level = std::make_shared<Level>(player->level);
        level_tree.insert(player->level, level);
    }
    (level_tree.getInfo(player->level))->player_tree.insert(player->id, player);
    // level.player_tree.insert(player.id, player);
}

void PlayersManager::AddPlayerToGroupTree(Group& group, std::shared_ptr<Player>& player) {
    if(!group.level_tree.find(player->level)) {
        // Level* new_level = new Level(player.level);
        std::shared_ptr<Level> new_level = std::make_shared<Level>(player->level);
        group.level_tree.insert(player->level, new_level);
    }
    (group.level_tree.getInfo(player->level))->player_tree.insert(player->id, player);
    // max player level maintaince
    if(player->level > group.max_level_player.level) {
        group.max_level_player.id = player->id;
        group.max_level_player.level = player->level;
    }
}

Array<Node<int, std::shared_ptr<Level>>*> PlayersManager::removeDuplicates(Array<Node<int, std::shared_ptr<Level>>*>& list) {
    int real_size = 1;
    int size = list.getSize();
    for(int i = 0; i < size; ++i) {  // O(m+n)
        real_size += list[i]->key != list[i+1]->key;
    }
    Array<Node<int, std::shared_ptr<Level>>*> new_list(real_size);
    for(int i=0; i <= size; ++i) {  // O(m+n)
        if((i < size && list[i]->key != list[i+1]->key) || (i == size && list[i-1]->key != list[i]->key)) {
            new_list.push_back(list[i]);
        }
        else {
            AVL<int, std::shared_ptr<Player>> merged = AVL<int, std::shared_ptr<Player>>::merge(
                list[i]->info->player_tree, list[i+1]->info->player_tree);  // O(m+n)
            std::shared_ptr<Level> level = std::make_shared<Level>(list[i]->key, merged);
            list[i]->info = level;
            new_list.push_back(list[i]);
            i++;
        }
    }
    return new_list;
}

void PlayersManager::updatePlayersPointers(std::shared_ptr<Group>& group, std::shared_ptr<Node<int, std::shared_ptr<Player>>>& root) {
    if(!root) {
        return;
    }
    Player* player = (root->info).get();
    player->group = group.get();
    updatePlayersPointers(group, root->left);
    updatePlayersPointers(group, root->right);
}
void PlayersManager::updateGroupPointers(std::shared_ptr<Group>& group, std::shared_ptr<Node<int, std::shared_ptr<Level>>>& root) {
    if(!root) {
        return;
    }
    AVL<int, std::shared_ptr<Player>>& avl = (root->info)->player_tree;
    PlayersManager::updatePlayersPointers(group, avl.root);
    updateGroupPointers(group, root->left);
    updateGroupPointers(group, root->right);
}

void PlayersManager::updateMaxLevel(AVL<int, std::shared_ptr<Level>>& level_tree, MaxPlayerInfo& max_player_info) {
    std::shared_ptr<Level>& max_level = level_tree.getMax();  // O(logn)
    if(max_level->player_tree.number_of_nodes == 0) {
        max_player_info.id = -1;
        max_player_info.level = -1;
        return;
    }
    std::shared_ptr<Player>& max_player = max_level->player_tree.getMin();  // O(logn)
    max_player_info.id = max_player->id;
    max_player_info.level = max_player->level;
}
/** *******************************************************************************************************************
 *                                             MEMBER FUNCTIONS                                                       *
***********************************************************************************************************************/

StatusType PlayersManager::AddGroup(int groupid) {
    if(groupid <= 0) {
        return INVALID_INPUT;
    }
    try {
        if(group_tree.find(groupid)) {
            return FAILURE;
        }
        std::shared_ptr<Group> group = std::make_shared<Group>(groupid);
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
        if(group_tree.find(groupid) == nullptr || player_tree.find(playerid)) {  // O(logk) + log(logn)
            return FAILURE;
        }
        std::shared_ptr<Player> player = std::make_shared<Player>(playerid, level);
        Group* group = (group_tree.getInfo(groupid)).get();  // O(logk)
        player->group = group;
        player_tree.insert(playerid, player);
        PlayersManager::AddPlayerToGroupTree(*group, player);
        PlayersManager::AddPlayerToLevelTree(level_tree, player);

        // maintaince max level in all players
        if(level >= max_level_player.level) {
            if(level == max_level_player.level) {
                max_level_player.id = std::min(playerid, max_level_player.id);
            }
            else {
                max_level_player.id = playerid;
            }
            max_level_player.level = level;
        }
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
        if(player_tree.find(playerid) == nullptr) {  // O(logn)
            return FAILURE;
        }
        int player_level = player_tree.getInfo(playerid)->level;
        Group* group = (player_tree.getInfo(playerid))->group;  // O(logn)

        Level* group_level = group->level_tree.getInfo(player_level).get();
        group_level->player_tree.remove(playerid);  // O(logn)
        updateMaxLevel(group->level_tree, group->max_level_player);

        Level* level = level_tree.getInfo(player_level).get();
        level->player_tree.remove(playerid);  // O(logn)
        player_tree.remove(playerid);  // O(logn)

        if(level->player_tree.number_of_nodes == 0) {
            level_tree.remove(level->id);
        }
        if(group_level->player_tree.number_of_nodes == 0) {
            group->level_tree.remove(group_level->id);
        }
        updateMaxLevel(level_tree, this->max_level_player);
    }
    catch(const KeyDoesNotExist& e) {
        return FAILURE;
    }
    return SUCCESS;
}

StatusType PlayersManager::ReplaceGroup(int groupid, int replacementid) { // O(logk + n + m)
    if(groupid <= 0 || replacementid <= 0 || groupid == replacementid) {
        return FAILURE;
    }
    try {
        Group *g1 = (group_tree.getInfo(groupid)).get();
        Group *g2 = (group_tree.getInfo(replacementid)).get();
        if(g1->level_tree.number_of_nodes == 0)  {  // no players in group
            return SUCCESS;
        }
        auto merged_list = AVL<int, std::shared_ptr<Level>>::mergeToList(g1->level_tree, g2->level_tree);
        auto no_duplicates_list = removeDuplicates(merged_list);
        AVL<int, std::shared_ptr<Level>> merged = AVL<int, std::shared_ptr<Level>>::listToAVL(no_duplicates_list);
        std::shared_ptr<Node<int, std::shared_ptr<Level>>> root = merged.root;
        std::shared_ptr<Group> new_group = std::make_shared<Group>(replacementid, merged);
        PlayersManager::updateGroupPointers(new_group, root);
        group_tree.remove(groupid);
        group_tree.remove(replacementid);

        updateMaxLevel(new_group->level_tree, new_group->max_level_player);

        group_tree.insert(replacementid, new_group);
        
    }
    catch(const std::bad_alloc& e) {
        return ALLOCATION_ERROR;
    }
    return SUCCESS;
}

StatusType PlayersManager::IncreaseLevel(int playerid, int levelincrease) {
    if(playerid <= 0 || levelincrease <= 0) {
        return INVALID_INPUT;
    }
    try { // O(logn)
        std::shared_ptr<Player>& player = player_tree.getInfo(playerid);  // should throw if not found
        Level* level = level_tree.getInfo(player->level).get();  // O(logn)
        Level* level_group = player->group->level_tree.getInfo(player->level).get();  // O(logn)
        level->player_tree.remove(player->id);  // O(logn)
        level_group->player_tree.remove(player->id);  // O(logn)
        if(level->player_tree.number_of_nodes == 0) {
            level_tree.remove(level->id);
        }
        if(level_group->player_tree.number_of_nodes == 0) {
            player->group->level_tree.remove(level_group->id);
        }
        
        player->level += levelincrease;

        if(!level_tree.find(player->level)) {
            std::shared_ptr<Level> new_level_ptr = std::make_shared<Level>(player->level);
            level_tree.insert(player->level, new_level_ptr);
        }
        Level* new_level = level_tree.getInfo(player->level).get();  // O(logn)
        if(!player->group->level_tree.find(player->level)) {
            std::shared_ptr<Level> new_level_ptr = std::make_shared<Level>(player->level);
            player->group->level_tree.insert(player->level, new_level_ptr);
        }
        Level* new_level_group = player->group->level_tree.getInfo(player->level).get();  // O(logn)
        new_level->player_tree.insert(player->id, player);  // O(logn)
        new_level_group->player_tree.insert(player->id, player);  // O(logn)
        
        if(player->level >= this->max_level_player.level) {
            if(player->id == this->max_level_player.id) {
                this->max_level_player.id = player->id;
            }
            else {
                this->max_level_player.id = player->id;
            }
            this->max_level_player.level = player->level;
        }
        // updateMaxLevel(level_tree, this->max_level_player);  // O(logn)
        updateMaxLevel(player->group->level_tree, player->group->max_level_player);  // O(logn)
    }
    catch(std::bad_alloc& e) {
        return ALLOCATION_ERROR;
    }
    catch(KeyDoesNotExist& e) {
        return FAILURE;
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
    assert(pm.AddPlayer(3, 2, 2) == SUCCESS);
    assert(pm.AddPlayer(4, 3, 1) == SUCCESS);
    assert(pm.AddPlayer(5, 5, 1) == SUCCESS);
    assert(pm.AddPlayer(1, 2 ,1) == FAILURE);
    assert(pm.AddPlayer(1, 4 ,1) == FAILURE);
    assert(pm.AddPlayer(1, 0 ,1) == INVALID_INPUT);
    assert(pm.RemovePlayer(1) == SUCCESS);
    assert(pm.RemovePlayer(1) == FAILURE);
    assert(pm.AddPlayer(1, 2, 1) == SUCCESS);
    assert(pm.ReplaceGroup(2, 3) == SUCCESS);
    assert(pm.AddGroup(2) == SUCCESS);
    assert(pm.AddGroup(3) == FAILURE);

    assert(pm.AddPlayer(1, 2, 1) == FAILURE);
    assert(pm.AddPlayer(4, 2, 1) == FAILURE);
    assert(pm.AddPlayer(1, 4, 1) == FAILURE);
    assert(pm.AddPlayer(5, 5, 1) == FAILURE);

    assert(pm.IncreaseLevel(1, 4) == SUCCESS);
    assert(pm.IncreaseLevel(5, 7) == SUCCESS);
    assert(pm.IncreaseLevel(11, 2) == FAILURE);
    assert(pm.IncreaseLevel(3, 1) == SUCCESS);
    assert(pm.RemovePlayer(5) == SUCCESS);

    int temp = 1;
}