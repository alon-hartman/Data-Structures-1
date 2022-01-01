#ifndef RANKTREE_H
#define RANKTREE_H
#include "DynamicHashTable.h"
#include "Player.h"
#include <memory>

const int MAX_SCORE = 200;

struct TreeNode {
    int level_id;
    int height = 0;
    DHT players_in_level;
    int* scores_hist;
    int player_in_subtree;
    double average_level_in_subtree;
    std::shared_ptr<TreeNode> left;
    std::shared_ptr<TreeNode> right;
    
    TreeNode(int level=0, int height=0, int w=0) : 
        level_id(level), height(height), players_in_level(), average_level_in_subtree(0),
        scores_hist(new int[MAX_SCORE]{0}), player_in_subtree(0), left(nullptr), right(nullptr) { }
    ~TreeNode() {
        delete[] scores_hist;
        left = nullptr;
        right = nullptr;
    }
};

class RankTree {
    private:
        static inline int max(const int a, const int b) {
            return (a > b) ? a : b;
        }

        static int height(const std::shared_ptr<TreeNode>& root) {
            if(!root) {
                return -1;
            }
            return root->height;
        }

        static int get_balance_factor(const std::shared_ptr<TreeNode>& root) {
            return height(root->left) - height(root->right);
        }

        static void decrease_counts(std::shared_ptr<TreeNode>& A, std::shared_ptr<TreeNode>& B) {
            // A-B
            for (int i = 0; i < MAX_SCORE; ++i) {
                A->scores_hist[i] -= B->scores_hist[i];
            }
        }

        static void increase_counts(std::shared_ptr<TreeNode>& A, std::shared_ptr<TreeNode>& B) {
            // A+B
            for (int i = 0; i < MAX_SCORE; ++i) {
                A->scores_hist[i] += B->scores_hist[i];
            }
        }

        static void RR_rotation(std::shared_ptr<TreeNode>& parent) {
            std::shared_ptr<TreeNode> child = parent->right;
            decrease_counts(parent, child);  // A-B
            decrease_counts(child, child->left);  // B-B->left
            parent->right = child->left;
            parent->height = 1 + max(height(parent->left), height(parent->right));
            child->height = 1 + max(height(child->right), height(parent));
            child->left = parent;
            parent = child; //parent = B, parent->left = A
            
            // A->players_in_subtree = A->number_of_players + A->left.players_in_subtree + A->right.players_in_subtree
            std::shared_ptr<TreeNode>& A = parent->left;
            std::shared_ptr<TreeNode>& B = parent;
            A->player_in_subtree = A->players_in_level.number_of_players + A->left->player_in_subtree + A->right->player_in_subtree;
            increase_counts(A, A->right);  // A+A->right
            increase_counts(B, A);  // B+A
            A->average_level_in_subtree = ((A->left->average_level_in_subtree * A->left->player_in_subtree) + 
                                           (A->right->average_level_in_subtree * A->right->player_in_subtree) + 
                                            (A->level_id * A->players_in_level.number_of_players)) / A->player_in_subtree;
            B->average_level_in_subtree = ((A->average_level_in_subtree * A->player_in_subtree) + 
                                           (B->right->average_level_in_subtree * B->right->player_in_subtree) + 
                                            (B->level_id * B->players_in_level.number_of_players)) / B->player_in_subtree;
            
        }
        static void RL_rotation(std::shared_ptr<TreeNode>& parent) {
            LL_rotation(parent->right);
            RR_rotation(parent);
        }
        static void LL_rotation(std::shared_ptr<TreeNode>& parent)
        {
            std::shared_ptr<TreeNode> child = parent->left;
            std::shared_ptr<TreeNode> tmp = child->right;
            child->right = parent;
            parent->left = tmp;
            parent->height = 1 + max(height(parent->left), height(parent->right));
            child->height = 1 + max(height(child->left), height(child->right));
            parent = child;
        }
        static void LR_rotation(std::shared_ptr<TreeNode>& parent) {
            RR_rotation(parent->left);
            LL_rotation(parent);
        }

        static void balance_aux(std::shared_ptr<TreeNode>& root) {
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

        static const std::shared_ptr<TreeNode>& find_aux(const std::shared_ptr<TreeNode>& root, const int level_id) {
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

        static void insert_level_aux(std::shared_ptr<TreeNode>& root, const int level_id) {
            if(!root) {
                root = std::make_shared<TreeNode>(level_id);
                return;
            }
            else if(level_id < root->level_id) {
                insert_level_aux(root->left, level_id);
            }
            else {  // key > root->key
                insert_level_aux(root->right, level_id);
            }
            balance_aux(root);
            root->height = 1 + max(height(root->left), height(root->right));
        }

        static void insert_player_aux(std::shared_ptr<TreeNode>& root, const int level_id, std::shared_ptr<Player>& player) {
            root->average_level_in_subtree = (root->average_level_in_subtree*root->player_in_subtree + player->level)/(root->average_level_in_subtree+1);
            root->player_in_subtree++;
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
    
    public:
        std::shared_ptr<TreeNode> root;
        int number_of_levels = 1;
        std::shared_ptr<TreeNode> level_zero;
        RankTree() : root(std::make_shared<TreeNode>(0)), number_of_levels(1), level_zero(root) { }
        
        const std::shared_ptr<TreeNode>& find(const int level_id) const {
            return find_aux(root, level_id);
        }
        void insert(std::shared_ptr<Player> player) {
            if(!find(player->level)) {
                insert_level_aux(root, player->level);
            }
            insert_player_aux(root, player->level, player);
        }
};

#endif