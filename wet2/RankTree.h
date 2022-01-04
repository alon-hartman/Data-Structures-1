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
    int players_in_subtree;
    double average_level_in_subtree;
    std::shared_ptr<TreeNode> left;
    std::shared_ptr<TreeNode> right;
    
    TreeNode(int level=0) : 
        level_id(level), height(0), players_in_level(), average_level_in_subtree(0),
        scores_hist(new int[MAX_SCORE]{0}), players_in_subtree(0), left(nullptr), right(nullptr) { }
    ~TreeNode() {
        delete[] scores_hist;
        left = nullptr;
        right = nullptr;
    }
};

class RankTree {
    private:
        static int max(const int a, const int b);
        static int height(const std::shared_ptr<TreeNode>& root);
        static int get_balance_factor(const std::shared_ptr<TreeNode>& root);
        static void decrease_counts(std::shared_ptr<TreeNode>& A, std::shared_ptr<TreeNode>& B);
        static void increase_counts(std::shared_ptr<TreeNode>& A, std::shared_ptr<TreeNode>& B);
        static double sumOfLevelsInSubtree(std::shared_ptr<TreeNode>& root);
        static int getPlayersInSubtree(std::shared_ptr<TreeNode>& root);
        static void recalculate_average(std::shared_ptr<TreeNode>& root);
        static void RR_rotation(std::shared_ptr<TreeNode>& parent);
        static void RL_rotation(std::shared_ptr<TreeNode>& parent);
        static void LL_rotation(std::shared_ptr<TreeNode>& parent);
        static void LR_rotation(std::shared_ptr<TreeNode>& parent);
        static void balance_aux(std::shared_ptr<TreeNode>& root);
        static const std::shared_ptr<TreeNode>& find_aux(const std::shared_ptr<TreeNode>& root, const int level_id);
        static void insert_level_aux(std::shared_ptr<TreeNode>& root, const int level_id, const std::shared_ptr<Player>& player);
        static void insert_player_aux(std::shared_ptr<TreeNode>& root, const int level_id, const std::shared_ptr<Player>& player);

    public:
        std::shared_ptr<TreeNode> root;
        int number_of_levels = 1;
        std::shared_ptr<TreeNode> level_zero;
        const std::shared_ptr<TreeNode>& find(const int level_id) const;
        void insert(std::shared_ptr<Player> player);
        RankTree();
};

#endif