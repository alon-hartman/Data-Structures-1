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

/** PUBLIC MEMBER FUNCTUINS */

RankTree::RankTree() : root(std::make_shared<TreeNode>(0)), number_of_levels(1), level_zero(root) { }
const std::shared_ptr<TreeNode>& RankTree::find(const int level_id) const {
    return find_aux(root, level_id);
}
void RankTree::insert(std::shared_ptr<Player> player) {
    if(!find(player->level)) {
        insert_level_aux(root, player->level, player);
        number_of_levels++;
        return;
    }
    insert_player_aux(root, player->level, player);
}
