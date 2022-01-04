#include "RankTree.h"

int RankTree::max(const int a, const int b) {
    return (a > b) ? a : b;
}

int RankTree::height(const std::shared_ptr<TreeNode>& root) {
    if(!root) {
        return -1;
    }
    return root->height;
}

int RankTree::get_balance_factor(const std::shared_ptr<TreeNode>& root) {
    return height(root->left) - height(root->right);
}

void RankTree::decrease_counts(std::shared_ptr<TreeNode>& A, std::shared_ptr<TreeNode>& B) {
    // A-B
    if(!A || !B) {
        return;
    }
    for (int i = 0; i < MAX_SCORE; ++i) {
        A->scores_hist[i] -= B->scores_hist[i];
    }
}

void RankTree::increase_counts(std::shared_ptr<TreeNode>& A, std::shared_ptr<TreeNode>& B) {
    // A+B
    if(!A || !B) {
        return;
    }
    for (int i = 0; i < MAX_SCORE; ++i) {
        A->scores_hist[i] += B->scores_hist[i];
    }
}

void RankTree::decrease_counts(int* A, int* B) {
    if(!A || !B) {
        return;
    }
    for (int i = 0; i < MAX_SCORE; ++i) {
        A[i] -= B[i];
    }
}
void RankTree::increase_counts(int* A, int* B) {
    if(!A || !B) {
        return;
    }
    for (int i = 0; i < MAX_SCORE; ++i) {
        A[i] += B[i];
    }
}

double RankTree::sumOfLevelsInSubtree(std::shared_ptr<TreeNode>& root) {
    if(!root) {
        return 0;
    }
    return root->average_level_in_subtree * root->players_in_subtree;
}
int RankTree::getPlayersInSubtree(std::shared_ptr<TreeNode>& root) {
    if(!root) {
        return 0;
    }
    return root->players_in_subtree;
}

void RankTree::recalculate_average(std::shared_ptr<TreeNode>& root) {
    if(!root) {
        return;
    }
    root->players_in_subtree = root->players_in_level.number_of_players + getPlayersInSubtree(root->left) + getPlayersInSubtree(root->right);
    if(root->players_in_subtree == 0) {
        root->average_level_in_subtree = 0;
        return;
    }
    root->average_level_in_subtree = (sumOfLevelsInSubtree(root->left) +
                                     (sumOfLevelsInSubtree(root->right)) +
                                     (root->level_id * root->players_in_level.number_of_players)) / root->players_in_subtree;
}

void RankTree::RR_rotation(std::shared_ptr<TreeNode>& parent) {
    std::shared_ptr<TreeNode> child = parent->right;
    decrease_counts(parent, child);  // A-B
    decrease_counts(child, child->left);  // B-B->left
    parent->right = child->left;
    parent->height = 1 + max(height(parent->left), height(parent->right));
    child->height = 1 + max(height(child->right), height(parent));
    child->left = parent;
    parent = child; //parent = B, parent->left = A
    
    std::shared_ptr<TreeNode>& A = parent->left;
    std::shared_ptr<TreeNode>& B = parent;
    increase_counts(A, A->right);  // A+A->right
    increase_counts(B, A);  // B+A
    recalculate_average(A);
    recalculate_average(B);    
}
void RankTree::RL_rotation(std::shared_ptr<TreeNode>& parent) {
    LL_rotation(parent->right);
    RR_rotation(parent);
}
void RankTree::LL_rotation(std::shared_ptr<TreeNode>& parent)
{
    std::shared_ptr<TreeNode> child = parent->left;
    std::shared_ptr<TreeNode> tmp = child->right;
    decrease_counts(parent, child); // B-A
    decrease_counts(child, child->right); // A-A->right
    child->right = parent;
    parent->left = tmp;
    parent->height = 1 + max(height(parent->left), height(parent->right));
    child->height = 1 + max(height(child->left), height(child->right));
    parent = child;
    std::shared_ptr<TreeNode>& B = parent->right;
    std::shared_ptr<TreeNode>& A = parent;
    increase_counts(B, B->left);  // B+B->left
    increase_counts(A, B);  // B+A
    recalculate_average(B);
    recalculate_average(A);
}
void RankTree::LR_rotation(std::shared_ptr<TreeNode>& parent) {
    RR_rotation(parent->left);
    LL_rotation(parent);
}

void RankTree::balance_aux(std::shared_ptr<TreeNode>& root) {
    if(!root) {
        return;
    }

    int balance_factor = get_balance_factor(root);
    if(balance_factor == 2) {
        if(get_balance_factor(root->left) >= 0) {
            LL_rotation(root);
        }
        else if (get_balance_factor(root->left) == -1) {
            LR_rotation(root);
        }
    }
    else if(balance_factor == -2) {
        if(get_balance_factor(root->right) <= 0) {
            RR_rotation(root);
        }
        else if(get_balance_factor(root->right) == 1) {
            RL_rotation(root);
        }
    }
}

const std::shared_ptr<TreeNode>& RankTree::find_aux(const std::shared_ptr<TreeNode>& root, const int level_id) {
    if(!root || level_id == root->level_id) {
        return root;
    }
    else if(level_id < root->level_id) {
        return find_aux(root->left, level_id);
    }
    else {  //(key > root->key) {
        return find_aux(root->right, level_id);
    }
}

void RankTree::insert_level_aux(std::shared_ptr<TreeNode>& root, const int level_id, const std::shared_ptr<Player>& player) {
    if(!root) {
        root = std::make_shared<TreeNode>(level_id);
        insert_player_aux(root, player->level, player);
        return;
    }
    else if(level_id < root->level_id) {
        insert_level_aux(root->left, level_id, player);
    }
    else {  // key > root->key
        insert_level_aux(root->right, level_id, player);
    }
    root->average_level_in_subtree = (root->average_level_in_subtree*root->players_in_subtree + player->level)/(root->players_in_subtree+1);
    root->players_in_subtree++;
    root->scores_hist[player->score]++;
    balance_aux(root);
    root->height = 1 + max(height(root->left), height(root->right));
}

void RankTree::insert_player_aux(std::shared_ptr<TreeNode>& root, const int level_id, const std::shared_ptr<Player>& player) {
    root->average_level_in_subtree = (root->average_level_in_subtree*root->players_in_subtree + player->level)/(root->players_in_subtree+1);
    root->players_in_subtree++;
    root->scores_hist[player->score]++;
    if(level_id == root->level_id) {
        root->players_in_level.addPlayer(player);
        return;
    }
    else if(level_id < root->level_id) {
        insert_player_aux(root->left, level_id, player);
        return;
    }
    else {  //(key > root->key) {
        insert_player_aux(root->right, level_id, player);
        return;
    }
}

void RankTree::remove_player_aux(std::shared_ptr<TreeNode>& root, std::shared_ptr<Player>& player) {
    if(!root) {
        return;
    }
    if(player->level < root->level_id) {
        remove_player_aux(root->left, player);
        root->height = 1 + max(height(root->left), height(root->right));
    }
    else if(root->level_id < player->level) {
        remove_player_aux(root->right, player);
        root->height = 1 + max(height(root->left), height(root->right));
    }
    else {  // key == root->key
        root->players_in_level.removePlayer(player);  // O(logn) to get here, O(1) to delete
    }
    // assuming player is always found
    if(root->players_in_subtree - 1 == 0) {
        root->average_level_in_subtree = 0;
    }
    else {
        root->average_level_in_subtree = (root->average_level_in_subtree*root->players_in_subtree - player->level)/(root->players_in_subtree-1);
    }
    root->players_in_subtree--;
    root->scores_hist[player->score]--;
    
}

void RankTree::remove_level_aux(std::shared_ptr<TreeNode>& root, const int level_id) {
    if(!root) {
        return;
    }
    if(level_id < root->level_id) {
        remove_level_aux(root->left, level_id);
        root->height = 1 + max(height(root->left), height(root->right));
    }
    else if(root->level_id < level_id) {
        remove_level_aux(root->right, level_id);
        root->height = 1 + max(height(root->left), height(root->right));
    }
    else {  // key == root->key
        // root is a leaf
        if(!root->left && !root->right) {
            root = nullptr;//.reset();
        }
        // root has left child
        else if(!root->right) {
            root = root->left;
        }
        // root has right child
        else if(!root->left) {  
            root = root->right;
        }
        // root has both childs
        else {
            std::shared_ptr<TreeNode> next = root->right;
            while(next->left) {
                next = next->left;
            }
            root->swap_data(*next.get());;
            recalculate_average(next);
            recalculate_average(root);
            int* temp_hist = new int[200]{0};
            increase_counts(temp_hist, root->scores_hist);
            if(next->right) {
                decrease_counts(temp_hist, next->right->scores_hist);
            }
            remove_level_and_fix_hist_aux(root->right, next->level_id, temp_hist);
            increase_counts(temp_hist, root->left->scores_hist);
            if(root->right) {
                increase_counts(temp_hist, root->right->scores_hist);
            }
            delete[] root->scores_hist;
            root->scores_hist = temp_hist;
        }
    }
    balance_aux(root);
}

void RankTree::remove_level_and_fix_hist_aux(std::shared_ptr<TreeNode>& root, const int level_id, int* hist) {
    if(!root) {
        return;
    }
    if(level_id < root->level_id) {
        remove_level_and_fix_hist_aux(root->left, level_id, hist);
        root->height = 1 + max(height(root->left), height(root->right));
    }
    else if(root->level_id < level_id) { //might be irrelevent
        remove_level_and_fix_hist_aux(root->right, level_id, hist);
        root->height = 1 + max(height(root->left), height(root->right));
    }
    else {
        // found and must not have left child, so
        root = root->right;
        return;
    }
    decrease_counts(root->scores_hist, hist);
}

/************************************** PUBLIC MEMBER FUNCTUINS **************************************/

RankTree::RankTree() : root(std::make_shared<TreeNode>(0)), number_of_levels(1), level_zero(root) { }
const std::shared_ptr<TreeNode>& RankTree::find(const int level_id) const {
    return find_aux(root, level_id);
}
void RankTree::insert(std::shared_ptr<Player>& player) {
    if(!find(player->level)) {
        insert_level_aux(root, player->level, player);
        number_of_levels++;
        return;
    }
    insert_player_aux(root, player->level, player);
}

void RankTree::removePlayer(std::shared_ptr<Player>& player) {
    std::shared_ptr<TreeNode> node = find(player->level);  // O(logn)
    if(!node) {
        return;
    }
    remove_player_aux(root, player);  // O(logn)
    if(node->players_in_level.number_of_players == 0 && node->level_id != 0) {
        remove_level_aux(root, player->level);  // O(logn)
        number_of_levels--;
    }
}