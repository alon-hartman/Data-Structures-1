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
}

void PlayersManager::AddPlayerToGroupTree(Group& group, std::shared_ptr<Player>& player) {
    if(!group.level_tree.find(player->level)) {
        // Level* new_level = new Level(player.level);
        std::shared_ptr<Level> new_level = std::make_shared<Level>(player->level);
        group.level_tree.insert(player->level, new_level);
    }
    (group.level_tree.getInfo(player->level))->player_tree.insert(player->id, player);
    group.size++;
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
    if(level_tree.number_of_nodes == 0) {
        max_player_info.id = -1;
        max_player_info.level = -1;
        return;
    }
    // if here then there is at least one node, which means the node isn't empty which means the following operations
    // must succeed (as long as this function is called after removed empty nodes).
    std::shared_ptr<Level>& max_level = level_tree.getMax();  // O(logn)
    std::shared_ptr<Player>& max_player = max_level->player_tree.getMin();  // O(logn)
    max_player_info.id = max_player->id;
    max_player_info.level = max_player->level;
}
/** *******************************************************************************************************************
 *                                             MEMBER FUNCTIONS                                                       *
***********************************************************************************************************************/

PMStatusType PlayersManager::AddGroup(int groupid) { //O(logk)
    if(groupid <= 0) {
        return PM_INVALID_INPUT;
    }
    try {
        if(group_tree.find(groupid)) {
            return PM_FAILURE;
        }
        std::shared_ptr<Group> group = std::make_shared<Group>(groupid);
        group_tree.insert(groupid, group);
    }
    catch(const std::bad_alloc& e) {
        return PM_ALLOCATION_ERROR;
    }
    catch(const KeyAlreadyExists& e) {
        return PM_FAILURE;
    }
    return PM_SUCCESS;
}

PMStatusType PlayersManager::AddPlayer(const int playerid, const int groupid, const int level) { // O(logk) + log(logn)
    if(playerid <= 0 || groupid <= 0 || level < 0) {
        return PM_INVALID_INPUT;
    }
    int allocations = 0;
    if(group_tree.find(groupid) == nullptr || player_tree.find(playerid)) {  // O(logk) + log(logn)
        return PM_FAILURE;
    }
    std::shared_ptr<Player> player = std::make_shared<Player>(playerid, level);
    std::shared_ptr<Group>& group = group_tree.getInfo(groupid);  // O(logk)
    player->group = group.get();
    try {
        player_tree.insert(playerid, player);
        allocations++;
        PlayersManager::AddPlayerToGroupTree(*(group.get()), player);
        allocations++;
        PlayersManager::AddPlayerToLevelTree(level_tree, player);
        allocations++;
        if(group->size == 1) {
            not_empty_group_tree.insert(group->id, group);  // O(logn)
        }

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
        if(allocations == 1) {
            player_tree.remove(playerid);
        }
        else if(allocations == 2) {
            player_tree.remove(playerid);
            Level* level_obj = group->level_tree.getInfo(level).get();  // O(logn)
            level_obj->player_tree.remove(playerid);  // O(logn)
            group->size--;
        }
        else if(allocations == 3) {
            RemovePlayer(playerid);
        }
        return PM_ALLOCATION_ERROR;
    }
    catch(const KeyAlreadyExists& e) {
        return PM_FAILURE;
    }
    return PM_SUCCESS;
}

PMStatusType PlayersManager::RemovePlayer(int playerid) { // O(logn)
    if(playerid <= 0) {
        return PM_INVALID_INPUT;
    }
    try {
        if(player_tree.find(playerid) == nullptr) {  // O(logn)
            return PM_FAILURE;
        }
        int player_level = player_tree.getInfo(playerid)->level;
        Group* group = (player_tree.getInfo(playerid))->group;  // O(logn)
        Level* group_level = group->level_tree.getInfo(player_level).get();
        group_level->player_tree.remove(playerid);  // O(logn)

        Level* level = level_tree.getInfo(player_level).get();
        level->player_tree.remove(playerid);  // O(logn)
        player_tree.remove(playerid);  // O(logn)

        group->size--;
        if(group->size == 0) {
            not_empty_group_tree.remove(group->id);  // O(logn) because the n must be larger than the number of not empty groups (each group has atleast one player)
        }

        if(level->player_tree.number_of_nodes == 0) {
            level_tree.remove(level->id);
        }
        if(group_level->player_tree.number_of_nodes == 0) {
            group->level_tree.remove(group_level->id);
        }
        updateMaxLevel(group->level_tree, group->max_level_player);
        updateMaxLevel(level_tree, this->max_level_player);
    }
    catch(const KeyDoesNotExist& e) {
        return PM_FAILURE;
    }
    return PM_SUCCESS;
}

PMStatusType PlayersManager::ReplaceGroup(int groupid, int replacementid) { // O(logk + n + m)
    if(groupid <= 0 || replacementid <= 0 || groupid == replacementid) {
        return PM_FAILURE;
    }
    try {
        Group *g1 = (group_tree.getInfo(groupid)).get();
        Group *g2 = (group_tree.getInfo(replacementid)).get();
        if(g1->level_tree.number_of_nodes == 0)  {  // no players in group
            return PM_SUCCESS;
        }
        auto merged_list = AVL<int, std::shared_ptr<Level>>::mergeToList(g1->level_tree, g2->level_tree);
        auto no_duplicates_list = removeDuplicates(merged_list);
        AVL<int, std::shared_ptr<Level>> merged = AVL<int, std::shared_ptr<Level>>::listToAVL(no_duplicates_list);
        std::shared_ptr<Node<int, std::shared_ptr<Level>>> root = merged.root;
        std::shared_ptr<Group> new_group = std::make_shared<Group>(replacementid, merged);
        new_group->size = g1->size + g2->size;

        PlayersManager::updateGroupPointers(new_group, root);
        group_tree.remove(groupid);
        group_tree.remove(replacementid);
        not_empty_group_tree.remove(groupid);
        not_empty_group_tree.remove(replacementid);

        updateMaxLevel(new_group->level_tree, new_group->max_level_player);

        group_tree.insert(replacementid, new_group);
        not_empty_group_tree.insert(replacementid, new_group);
        
    }
    catch(const std::bad_alloc& e) {
        return PM_ALLOCATION_ERROR;
    }
    return PM_SUCCESS;
}

PMStatusType PlayersManager::IncreaseLevel(int playerid, int levelincrease) { //O(logn)
    if(playerid <= 0 || levelincrease <= 0) {
        return PM_INVALID_INPUT;
    }
    /** TODO: handle allocation failures (return tree to previous state)*/

    // bool new_level_tree = false;
    // bool new_group_level_tree = false;
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

        if(!level_tree.find(player->level)) {  // create and insert new level to level_tree
            std::shared_ptr<Level> new_level_ptr = std::make_shared<Level>(player->level);
            level_tree.insert(player->level, new_level_ptr);
            // new_level_tree = true;
        }
        Level* new_level = level_tree.getInfo(player->level).get();  // O(logn)
        if(!player->group->level_tree.find(player->level)) {  // create and insert new level to group->level_tree
            std::shared_ptr<Level> new_level_ptr = std::make_shared<Level>(player->level);
            player->group->level_tree.insert(player->level, new_level_ptr);
            // new_group_level_tree = true;
        }
        Level* new_level_group = player->group->level_tree.getInfo(player->level).get();  // O(logn)
        new_level->player_tree.insert(player->id, player);  // O(logn)
        new_level_group->player_tree.insert(player->id, player);  // O(logn)
        
        if(player->level >= this->max_level_player.level) {
            if(player->level == this->max_level_player.level) {
                this->max_level_player.id = std::min(player->id, max_level_player.id);
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
        return PM_ALLOCATION_ERROR;
    }
    catch(KeyDoesNotExist& e) {
        return PM_FAILURE;
    }
    return PM_SUCCESS;
}

PMStatusType PlayersManager::GetHighestLevel(int groupid, int* playerid) {
    if(groupid == 0 || playerid == nullptr) {
        return PM_INVALID_INPUT;
    }
    if(groupid < 0) {
        *playerid = max_level_player.id;
        return PM_SUCCESS;
    }
    try {
        Group* group = group_tree.getInfo(groupid).get();
        *playerid = group->max_level_player.id;
        return PM_SUCCESS;
    }
    catch(const KeyDoesNotExist& e) {
        return PM_FAILURE;
    }
}

void PlayersManager::InorderPlayerTree(Array<int>& Players, std::shared_ptr<Node<int, std::shared_ptr<Player>>>& root) {
    if(!root) {
        return;
    }
    InorderPlayerTree(Players, root->left);
    Players.push_back(root->key);
    InorderPlayerTree(Players, root->right);
}

void PlayersManager::ReverseInorderLevelTree(Array<int>& Players, std::shared_ptr<Node<int, std::shared_ptr<Level>>>& root) {
    if(!root) {
        return;
    }
    ReverseInorderLevelTree(Players, root->right);
    InorderPlayerTree(Players, root->info->player_tree.root);
    ReverseInorderLevelTree(Players, root->left);
}

// int* PlayersManager::GetAllPlayersByLevelAux(int num_of_players, AVL<int, std::shared_ptr<Level>>& level_tree) {
//     Array<int> array(num_of_players);
//     int* players = (int*)malloc(sizeof(int)*num_of_players);
//     if(!players) {
//         throw std::bad_alloc();
//     }
//     ReverseInorderLevelTree(array, level_tree);
//     for(int i=0; i<=array.getSize(); ++i) {
//         players[i] = array[i];
//     }
//     return players;
// }

PMStatusType PlayersManager::GetAllPlayersByLevel(int groupid, int **Players, int *numOfPlayers) {
    if(groupid == 0 || Players == nullptr || numOfPlayers == nullptr) {
        return PM_INVALID_INPUT;
    }
    try {
        if(groupid < 0) {
            if(player_tree.number_of_nodes == 0) {
                *numOfPlayers = 0;
                *Players = nullptr;
                return PM_SUCCESS;
            }
            Array<int> array(player_tree.number_of_nodes);
            ReverseInorderLevelTree(array, level_tree.root);
            *Players = (int*)malloc(sizeof(int)*player_tree.number_of_nodes);
            if(!*Players) {
                return PM_ALLOCATION_ERROR;
            }
            *numOfPlayers = player_tree.number_of_nodes;
            for(int i=0; i<=array.getSize(); ++i) {
                (*Players)[i] = array[i];
            } 
        }
        else {
            Group* group = group_tree.getInfo(groupid).get();
            if(group->size == 0) {
                *numOfPlayers = 0;
                *Players = nullptr;
                return PM_SUCCESS;
            }
            Array<int> array(group->size);
            *Players = (int*)malloc(sizeof(int)*group->size);
            if(!*Players) {
                return PM_ALLOCATION_ERROR;
            }
            ReverseInorderLevelTree(array, group->level_tree.root);
            *numOfPlayers = group->size;
            for(int i=0; i<=array.getSize(); ++i) {
                (*Players)[i] = array[i];
            }
        }
    }
    catch(std::bad_alloc& e) {
        return PM_ALLOCATION_ERROR;
    }
    catch(KeyDoesNotExist& e) {
        return PM_FAILURE;
    }
    return PM_SUCCESS;
}

void PlayersManager::InorderGroupTree(Array<int>& array, std::shared_ptr<Node<int, std::shared_ptr<Group>>>& root, int* printed){
    if(!root || *printed == 0) {
        return;
    }
    InorderGroupTree(array, root->left, printed);
    if(printed > 0) {
        array.push_back(root->info->max_level_player.id);
        (*printed)--;
    }
    InorderGroupTree(array, root->right, printed);
}

PMStatusType PlayersManager::GetGroupsHighestLevel(int numOfGroups, int** Players) {
    if(numOfGroups < 1 || Players == nullptr) {
        return PM_INVALID_INPUT;
    }
    if(not_empty_group_tree.number_of_nodes < numOfGroups) {
        return PM_FAILURE;
    }
    try {
        Array<int> array(numOfGroups);
        *Players = (int*)malloc(sizeof(int)*numOfGroups);
        if(!*Players) {
            return PM_ALLOCATION_ERROR;
        }
        InorderGroupTree(array, not_empty_group_tree.root, &numOfGroups);
        for(int i=0; i<=array.getSize(); ++i) {
            (*Players)[i] = array[i];
        }
    }
    catch(std::bad_alloc& e) {
        return PM_ALLOCATION_ERROR;
    }
    return PM_SUCCESS;
}

// void PrintAll(int *playerIDs, int numOfPlayers) {
// 	if (numOfPlayers > 0) {
// 		std::cout << "Rank	||	Player" << std::endl;
// 	}

// 	for (int i = 0; i < numOfPlayers; i++) {
// 		std::cout << i + 1 << "\t||\t" << playerIDs[i] << std::endl;
// 	}
// 	std::cout << "and there are no more players!" << std::endl;

// 	free (playerIDs);
// }

// void PrintGroupsHighest(int *playerIDs, int numOfGroups) {
// 	if (numOfGroups > 0) {
// 		std::cout << "GroupIndex	||	Player" << std::endl;
// 	}

// 	for (int i = 0; i < numOfGroups; i++) {
// 		std::cout << i + 1 << "\t||\t" << playerIDs[i] << std::endl;
// 	}
// 	std::cout << "and there are no more players!" << std::endl;

// 	free (playerIDs);
// }

// int main() {
//     PlayersManager pm;
//     assert(pm.AddGroup(2) == PM_SUCCESS);
//     assert(pm.AddGroup(3) == PM_SUCCESS);
//     assert(pm.AddGroup(5) == PM_SUCCESS);
//     assert(pm.AddGroup(1) == PM_SUCCESS);
//     assert(pm.AddGroup(7) == PM_SUCCESS);
//     assert(pm.AddGroup(2) == PM_FAILURE);
//     int numOfPlayers;
//     int *playerIDs;
//     assert(pm.GetAllPlayersByLevel(-1, &playerIDs, &numOfPlayers) == PM_SUCCESS);
//     PrintAll(playerIDs, numOfPlayers);
//     int* res = new int(0);
//     assert(pm.GetHighestLevel(-1, res) == PM_SUCCESS);
//     assert(*res == -1);
//     assert(pm.AddPlayer(1, 2, 1) == PM_SUCCESS);
//     assert(pm.AddPlayer(2, 2, 1) == PM_SUCCESS);
//     assert(pm.AddPlayer(3, 2, 2) == PM_SUCCESS);
//     assert(pm.AddPlayer(4, 3, 1) == PM_SUCCESS);
//     assert(pm.AddPlayer(5, 5, 1) == PM_SUCCESS);
//     assert(pm.AddPlayer(1, 2 ,1) == PM_FAILURE);
//     assert(pm.AddPlayer(1, 4 ,1) == PM_FAILURE);
//     assert(pm.AddPlayer(1, 0 ,1) == PM_INVALID_INPUT);
//     assert(pm.RemovePlayer(1) == PM_SUCCESS);
//     assert(pm.RemovePlayer(1) == PM_FAILURE);
//     assert(pm.AddPlayer(1, 2, 1) == PM_SUCCESS);
//     assert(pm.ReplaceGroup(2, 3) == PM_SUCCESS);
//     assert(pm.AddGroup(2) == PM_SUCCESS);
//     assert(pm.AddGroup(3) == PM_FAILURE);

//     assert(pm.AddPlayer(1, 2, 1) == PM_FAILURE);
//     assert(pm.AddPlayer(4, 2, 1) == PM_FAILURE);
//     assert(pm.AddPlayer(1, 4, 1) == PM_FAILURE);
//     assert(pm.AddPlayer(5, 5, 1) == PM_FAILURE);

//     assert(pm.IncreaseLevel(1, 4) == PM_SUCCESS);
//     assert(pm.IncreaseLevel(5, 7) == PM_SUCCESS);
//     assert(pm.IncreaseLevel(11, 2) == PM_FAILURE);
//     assert(pm.IncreaseLevel(3, 1) == PM_SUCCESS);
//     *res = 0;
//     assert(pm.GetHighestLevel(-10, res) == PM_SUCCESS);
//     assert(*res == 5);
//     assert(pm.RemovePlayer(5) == PM_SUCCESS);
//     *res = 0;
//     assert(pm.GetHighestLevel(5, res) == PM_SUCCESS);
//     assert(*res == -1);
//     *res = 0;
//     assert(pm.GetHighestLevel(3, res) == PM_SUCCESS);
//     assert(*res = 1);
//     assert(pm.GetHighestLevel(100, res) == PM_FAILURE);
//     assert(pm.GetHighestLevel(0, res) == PM_INVALID_INPUT);
//     assert(pm.GetHighestLevel(2, nullptr) == PM_INVALID_INPUT);

//     delete res;

    
//     assert(pm.GetAllPlayersByLevel(3, &playerIDs, &numOfPlayers) == PM_SUCCESS);
//     PrintAll(playerIDs, numOfPlayers);
//     assert(pm.GetAllPlayersByLevel(5, &playerIDs, &numOfPlayers) == PM_SUCCESS);
//     PrintAll(playerIDs, numOfPlayers);
//     assert(pm.GetAllPlayersByLevel(11, &playerIDs, &numOfPlayers) == PM_FAILURE);
//     assert(pm.GetAllPlayersByLevel(3, nullptr, &numOfPlayers) == PM_INVALID_INPUT);
//     assert(pm.GetAllPlayersByLevel(3, &playerIDs, nullptr) == PM_INVALID_INPUT);
//     assert(pm.AddPlayer(13, 7, 3) == PM_SUCCESS);
//     assert(pm.GetAllPlayersByLevel(7, &playerIDs, &numOfPlayers) == PM_SUCCESS);
//     PrintAll(playerIDs, numOfPlayers);

//     assert(pm.GetAllPlayersByLevel(-1, &playerIDs, &numOfPlayers) == PM_SUCCESS);
//     PrintAll(playerIDs, numOfPlayers);

//     assert(pm.GetGroupsHighestLevel(1, &playerIDs) == PM_SUCCESS);
//     PrintGroupsHighest(playerIDs, 1);
//     assert(pm.GetGroupsHighestLevel(2, &playerIDs) == PM_SUCCESS);
//     PrintGroupsHighest(playerIDs, 2);
//     assert(pm.GetGroupsHighestLevel(3, &playerIDs) == PM_FAILURE);
// }