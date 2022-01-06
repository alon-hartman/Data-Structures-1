#ifndef RANKTREE_H
#define RANKTREE_H
#include "DynamicHashTable.h"
#include "Player.h"
#include "Array.h"
#include <memory>

const int MAX_SCORE = 200;

struct TreeNode {
    int level_id;
    int height = 0;
    DHT players_in_level;
    int scale;
    int* scores_hist;
    int players_in_subtree;
    double average_level_in_subtree;
    std::shared_ptr<TreeNode> left;
    std::shared_ptr<TreeNode> right;
    
    TreeNode(int scale, int level) : 
        level_id(level), height(0), players_in_level(), average_level_in_subtree(0), scale(scale),
        scores_hist(new int[scale]{0}), players_in_subtree(0), left(nullptr), right(nullptr) { }
    ~TreeNode() {
        delete[] scores_hist;
        left = nullptr;
        right = nullptr;
    }
    void swap_data(TreeNode& other) {
        // could implement a generic swap but
        int temp = level_id;
        level_id = other.level_id;
        other.level_id = temp;

        temp = height;
        height = other.height;
        other.height = temp;

        players_in_level.swap(other.players_in_level);
        int* _temp = scores_hist;
        scores_hist = other.scores_hist;
        other.scores_hist = _temp;

        temp = players_in_subtree;
        players_in_subtree = other.players_in_subtree;
        other.players_in_subtree = temp;

        temp = average_level_in_subtree;
        average_level_in_subtree = other.average_level_in_subtree;
        other.average_level_in_subtree = temp;
    }
    TreeNode& operator=(const TreeNode& other) {
        if(this == &other) {
            return *this;
        }
        this->level_id = other.level_id;
        this->height = other.height;
        this->players_in_level = other.players_in_level;
        delete[] this->scores_hist;
        this->scores_hist = other.scores_hist;
        this->players_in_subtree = other.players_in_subtree;
        this->average_level_in_subtree = other.average_level_in_subtree;
        this->left = other.left;
        this->right = other.right;
        return *this;
    }
};

class RankTree {
    private:
        static int max(const int a, const int b);
        static int height(const std::shared_ptr<TreeNode>& root);
        static int get_balance_factor(const std::shared_ptr<TreeNode>& root);
        static void decrease_counts(const std::shared_ptr<TreeNode>& A, const std::shared_ptr<TreeNode>& B);
        static void decrease_counts(int* A, int* B, int scale);
        static void increase_counts(const std::shared_ptr<TreeNode>& A, const std::shared_ptr<TreeNode>& B);
        static void increase_counts(int* A, int* B, int scale);
        static double sumOfLevelsInSubtree(std::shared_ptr<TreeNode>& root);
        static int getPlayersInSubtree(std::shared_ptr<TreeNode>& root);
        static void recalculate_average(std::shared_ptr<TreeNode>& root);
        static void RR_rotation(std::shared_ptr<TreeNode>& parent);
        static void RL_rotation(std::shared_ptr<TreeNode>& parent);
        static void LL_rotation(std::shared_ptr<TreeNode>& parent);
        static void LR_rotation(std::shared_ptr<TreeNode>& parent);
        static void balance_aux(std::shared_ptr<TreeNode>& root);
        static const std::shared_ptr<TreeNode>& find_aux(const std::shared_ptr<TreeNode>& root, const int level_id);
        static void insert_level_aux(std::shared_ptr<TreeNode>& root, const int level_id, const std::shared_ptr<Player>& player, int scale);
        static void insert_player_aux(std::shared_ptr<TreeNode>& root, const int level_id, const std::shared_ptr<Player>& player);
        static void remove_player_aux(std::shared_ptr<TreeNode>& root, std::shared_ptr<Player>& player);
        static void remove_level_aux(std::shared_ptr<TreeNode>& root, const int level_id);
        static void remove_level_and_fix_hist_aux(std::shared_ptr<TreeNode>& root, const int level_id, int* hist);

        static void inorderToList(const std::shared_ptr<TreeNode>& root, Array<std::shared_ptr<TreeNode>>& list);
        Array<std::shared_ptr<TreeNode>> getTreeAsList() const;
        static std::shared_ptr<TreeNode> getTreeFromListAux(const Array<std::shared_ptr<TreeNode>>& list, int start, int end);
        static RankTree getTreeFromList(const Array<std::shared_ptr<TreeNode>>& list, int scale);
        static Array<std::shared_ptr<TreeNode>> mergeToList(const RankTree& av1, const RankTree& avl2);
        static Array<std::shared_ptr<TreeNode>> removeDuplicates(const Array<std::shared_ptr<TreeNode>>& list);
        static RankTree listToRankTree(const Array<std::shared_ptr<TreeNode>>& list, int scale);
        static void updateMergedTree(std::shared_ptr<TreeNode>& root);
        static void resetHistogram(int* histogram);

        static int findUpperBound(std::shared_ptr<TreeNode>& root, const int level_id);
        static int findLowerBound(std::shared_ptr<TreeNode>& root, const int level_id);
        static void RankAndScoreRankAux(std::shared_ptr<TreeNode>& root, const int bound, const int score, int* const sum_players, int* const sum_score);
        static int getSubtreeAtScore(std::shared_ptr<TreeNode>& root, const int score);
        void update_zero_path(std::shared_ptr<TreeNode&>& root);

        int scale;

        
    public:
        std::shared_ptr<TreeNode> root;
        int number_of_levels = 1;
        std::shared_ptr<TreeNode> level_zero;

        RankTree(int scale);

        const std::shared_ptr<TreeNode>& find(const int level_id) const;
        void insert(std::shared_ptr<Player>& player);
        void removePlayer(std::shared_ptr<Player>& player);
        static RankTree merge(const RankTree& rt1, const RankTree& rt2);
        double getPercentOfPlayersWithScoreInBounds(const int lower, const int upper, const int score);
};

#endif