#ifndef RANKTREE_H
#define RANKTREE_H
#include "DynamicHashTable.h"
#include "Player.h"
#include "Array.h"
#include <memory>

const int MAX_SCORE = 200;

template<typename T>
void swap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

struct TreeNode {
    int level_id;
    int height = 0;
    int players_in_level;
    int scale;
    double average_level_in_subtree;
    int* scores_hist;
    int* self_scores_hist;
    int players_in_subtree;
    std::shared_ptr<TreeNode> left;
    std::shared_ptr<TreeNode> right;
    
    TreeNode(int scale, int level) : 
        level_id(level), height(0), players_in_level(0), scale(scale), average_level_in_subtree(0),
        scores_hist(new int[MAX_SCORE]{0}), self_scores_hist(new int[MAX_SCORE]{0}), players_in_subtree(0), left(nullptr), right(nullptr) { }
    ~TreeNode() {
        delete[] scores_hist;
        delete[] self_scores_hist;
        left = nullptr;
        right = nullptr;
    }
    void swap_data(TreeNode& other) {
        swap<int>(level_id, other.level_id);
        swap<int>(height, other.height);
        swap<int>(players_in_level, other.players_in_level);
        swap<int*>(scores_hist, other.scores_hist);
        swap<int*>(self_scores_hist, other.self_scores_hist);
        swap<int>(players_in_subtree, other.players_in_subtree);
        swap<double>(average_level_in_subtree, other.average_level_in_subtree);
        swap<int>(scale, other.scale);
    }
};

class RankTree {
    private:
        static int max(const int a, const int b);
        static int min(const int a, const int b);
        static int height(const std::shared_ptr<TreeNode>& root);
        static int get_balance_factor(const std::shared_ptr<TreeNode>& root);
        static void decrease_counts(const std::shared_ptr<TreeNode>& A, const std::shared_ptr<TreeNode>& B);
        static void decrease_counts(int* A, int* B, int scale);
        static void increase_counts(const std::shared_ptr<TreeNode>& A, const std::shared_ptr<TreeNode>& B);
        static void increase_counts(int* A, int* B, int scale);
        static double sumOfLevelsInSubtree(std::shared_ptr<TreeNode>& root);
        static int getPlayersInSubtree(std::shared_ptr<TreeNode>& root);
        static double getAverageLevelInSubtree(std::shared_ptr<TreeNode>& root);
        static void recalculate_average(std::shared_ptr<TreeNode>& root);
        static void RR_rotation(std::shared_ptr<TreeNode>& parent);
        static void RL_rotation(std::shared_ptr<TreeNode>& parent);
        static void LL_rotation(std::shared_ptr<TreeNode>& parent);
        static void LR_rotation(std::shared_ptr<TreeNode>& parent);
        static void balance_aux(std::shared_ptr<TreeNode>& root);
        static const std::shared_ptr<TreeNode>& find_level_aux(const std::shared_ptr<TreeNode>& root, const int level_id);
        static void insert_level_aux(std::shared_ptr<TreeNode>& root, const int level_id, const std::shared_ptr<Player>& player, int scale);
        static void insert_player_aux(std::shared_ptr<TreeNode>& root, const int level_id, const std::shared_ptr<Player>& player);
        static void remove_player_aux(std::shared_ptr<TreeNode>& root, std::shared_ptr<Player>& player);
        static void remove_level_aux(std::shared_ptr<TreeNode>& root, const int level_id);
        static void remove_level_and_fix_hist_aux(std::shared_ptr<TreeNode>& root, const int level_id, int* hist);

        static void inorderToList(const std::shared_ptr<TreeNode>& root, Array<std::shared_ptr<TreeNode>>& list);
        Array<std::shared_ptr<TreeNode>> getTreeAsList() const;
        static std::shared_ptr<TreeNode> getTreeFromListAux(const Array<std::shared_ptr<TreeNode>>& list, int start, int end);
        static RankTree* getTreeFromList(const Array<std::shared_ptr<TreeNode>>& list, int scale);
        static Array<std::shared_ptr<TreeNode>> mergeToList(const RankTree& av1, const RankTree& avl2);
        static Array<std::shared_ptr<TreeNode>> removeDuplicates(const Array<std::shared_ptr<TreeNode>>& list);
        static RankTree* listToRankTree(const Array<std::shared_ptr<TreeNode>>& list, int scale);
        static void updateMergedTree(std::shared_ptr<TreeNode>& root);
        static void resetHistogram(int* histogram, int scale);

        static int findUpperBound(std::shared_ptr<TreeNode>& root, const int level_id);
        static int findLowerBound(std::shared_ptr<TreeNode>& root, const int level_id);
        static void RankAndScoreRankAux(std::shared_ptr<TreeNode>& root, const int bound, const int score, int* const sum_players, int* const sum_score);
        static int getSubtreeAtScore(std::shared_ptr<TreeNode>& root, const int score);

        static double averageHighestPlayerLevelByGroupAux(std::shared_ptr<TreeNode>& root, int m);


        static void change_player_score_aux(std::shared_ptr<TreeNode>& root, const int level, const int old_score, const int new_score);

        static void getPlayersBoundsAux(std::shared_ptr<TreeNode>& root, int score, int m, int* lower, int* higher);

    public:
        int scale;
        std::shared_ptr<TreeNode> root;
        int number_of_levels = 1;  // starts with level 0
        std::shared_ptr<TreeNode> level_zero;

        RankTree(int scale);

        int getPlayersInTree();
        static void update_zero_path(std::shared_ptr<TreeNode>& root);

        const std::shared_ptr<TreeNode>& findLevel(const int level_id) const;
        void insert(std::shared_ptr<Player>& player);
        void removePlayer(std::shared_ptr<Player>& player);
        static RankTree* merge(const RankTree& rt1, const RankTree& rt2);
        double getPercentOfPlayersWithScoreInBounds(const int lower, const int upper, const int score);
        double averageHighestPlayerLevelByGroup(int m);

        void change_player_score(const int level, const int old_score, const int new_score);

        void getPlayersBounds(int score, int m, int* lower, int* higher);
};

#endif