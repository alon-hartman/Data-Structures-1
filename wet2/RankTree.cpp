#include "RankTree.h"
#include "Array.h"

/********************************** HELPER FUNCTIONS **********************************/

int RankTree::max(const int a, const int b) {
    return (a > b) ? a : b;
}

int RankTree::min(const int a, const int b) {
    return (a < b) ? a : b;
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

void RankTree::decrease_counts(const std::shared_ptr<TreeNode>& A, const std::shared_ptr<TreeNode>& B) {
    // A-B
    if(!A || !B) {
        return;
    }
    for (int i = 0; i < A->scale; ++i) {
        A->scores_hist[i] = max(0, A->scores_hist[i] - B->scores_hist[i]);
    }
}

void RankTree::increase_counts(const std::shared_ptr<TreeNode>& A, const std::shared_ptr<TreeNode>& B) {
    // A+B
    if(!A || !B) {
        return;
    }
    for (int i = 0; i < A->scale; ++i) {
        A->scores_hist[i] += B->scores_hist[i];
    }
}

void RankTree::decrease_counts(int* A, int* B, int scale) {
    if(!A || !B) {
        return;
    }
    for (int i = 0; i < scale; ++i) {
        A[i] = max(0, A[i]-B[i]);
    }
}
void RankTree::increase_counts(int* A, int* B, int scale) {
    if(!A || !B) {
        return;
    }
    for (int i = 0; i < scale; ++i) {
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

double RankTree::getAverageLevelInSubtree(std::shared_ptr<TreeNode>& root) {
    if(!root) {
        return 0;
    }
    return root->average_level_in_subtree;
}

void RankTree::recalculate_average(std::shared_ptr<TreeNode>& root) {
    if(!root) {
        return;
    }
    root->players_in_subtree = root->players_in_level + getPlayersInSubtree(root->left) + getPlayersInSubtree(root->right);
    if(root->players_in_subtree == 0) {
        root->average_level_in_subtree = 0;
        return;
    }
    root->average_level_in_subtree = (sumOfLevelsInSubtree(root->left) +
                                     (sumOfLevelsInSubtree(root->right)) +
                                     (root->level_id * root->players_in_level)) / root->players_in_subtree;
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

const std::shared_ptr<TreeNode>& RankTree::find_level_aux(const std::shared_ptr<TreeNode>& root, const int level_id) {
    if(!root || level_id == root->level_id) {
        return root;
    }
    else if(level_id < root->level_id) {
        return find_level_aux(root->left, level_id);
    }
    else {  //(key > root->key) {
        return find_level_aux(root->right, level_id);
    }
}

void RankTree::insert_level_aux(std::shared_ptr<TreeNode>& root, const int level_id, const std::shared_ptr<Player>& player, int scale) {
    if(!root) {
        root = std::make_shared<TreeNode>(scale, level_id);
        insert_player_aux(root, player->level, player);
        return;
    }
    else if(level_id < root->level_id) {
        insert_level_aux(root->left, level_id, player, scale);
    }
    else  {
        insert_level_aux(root->right, level_id, player, scale);
    }
    root->average_level_in_subtree = (root->average_level_in_subtree*root->players_in_subtree + player->level)/(root->players_in_subtree+1);
    root->players_in_subtree++;
    root->scores_hist[player->score]++;
    balance_aux(root);
    root->height = 1 + max(height(root->left), height(root->right));
}

void RankTree::change_player_score_aux(std::shared_ptr<TreeNode>& root, const int level, const int old_score, const int new_score) {
    if(!root) {
        return;
    }
    if(root->level_id < level) {
        change_player_score_aux(root->right, level, old_score, new_score);
    }
    else if(root->level_id > level) {
        change_player_score_aux(root->left, level, old_score, new_score);
    }
    else {
        root->self_scores_hist[old_score]--;
        root->self_scores_hist[new_score]++;
    }
    root->scores_hist[old_score]--;
    root->scores_hist[new_score]++;
}

void RankTree::insert_player_aux(std::shared_ptr<TreeNode>& root, const int level_id, const std::shared_ptr<Player>& player) {
    root->average_level_in_subtree = (root->average_level_in_subtree*root->players_in_subtree + player->level)/(root->players_in_subtree+1);
    root->players_in_subtree++;
    root->scores_hist[player->score]++;
    if(level_id == root->level_id) {
        root->players_in_level++;
        root->self_scores_hist[player->score]++;
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
        root->players_in_level--;
        root->self_scores_hist[player->score]--;
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
            root->swap_data(*next.get());

            remove_level_and_fix_hist_aux(root->right, next->level_id, root->self_scores_hist);
            recalculate_average(root);

            resetHistogram(root->scores_hist, root->scale);
            increase_counts(root->scores_hist, root->self_scores_hist, root->scale);
            increase_counts(root, root->left);
            increase_counts(root, root->right);
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
    recalculate_average(root);
    decrease_counts(root->scores_hist, hist, root->scale);
}

void RankTree::update_zero_path(std::shared_ptr<TreeNode>& root) {
    if(root->level_id == 0){
        return;
    }
    decrease_counts(root, root->left);
    update_zero_path(root->left);
    if(root->left->level_id == 0){
        resetHistogram(root->scores_hist, root->scale);
        increase_counts(root->scores_hist, root->self_scores_hist, root->scale);
        increase_counts(root, root->right);
        increase_counts(root, root->left);
    }
    else {
        increase_counts(root, root->left);
    }
    recalculate_average(root);
}

/********************************** MERGE FUNCTIONS **********************************/
void RankTree::inorderToList(const std::shared_ptr<TreeNode>& root, Array<std::shared_ptr<TreeNode>>& list) {
    if(!root) {
        return;
    }
    inorderToList(root->left, list);
    list.push_back(root);
    inorderToList(root->right, list);
}
Array<std::shared_ptr<TreeNode>> RankTree::getTreeAsList() const {
    Array<std::shared_ptr<TreeNode>> list(number_of_levels);
    inorderToList(root, list);  // O(n)
    return list; 
}
class NodesComparator {
    public:
        bool operator()(std::shared_ptr<TreeNode> left, std::shared_ptr<TreeNode> right) {
            return left->level_id < right->level_id;
        }
};

std::shared_ptr<TreeNode> RankTree::getTreeFromListAux(const Array<std::shared_ptr<TreeNode>>& list, int start, int end) {
    if(start > end) {
        return nullptr;
    }
    int mid = (start+end)/2;
    std::shared_ptr<TreeNode> root = list[mid];
    root->left = getTreeFromListAux(list, start, mid-1);
    root->right = getTreeFromListAux(list, mid+1, end);
    return root;
}

RankTree* RankTree::getTreeFromList(const Array<std::shared_ptr<TreeNode>>& list, int scale) {
    RankTree* rt = new RankTree(scale);
    rt->root = getTreeFromListAux(list, 0, list.getSize());
    rt->number_of_levels = list.getSize()+1;
    rt->level_zero = rt->findLevel(0);
    return rt;
}

Array<std::shared_ptr<TreeNode>> RankTree::mergeToList(const RankTree& rt1, const RankTree& rt2) {
    NodesComparator cmp;
    Array<std::shared_ptr<TreeNode>> list1 = rt1.getTreeAsList();  // O(n)
    Array<std::shared_ptr<TreeNode>> list2 = rt2.getTreeAsList();  // O(m)
    Array<std::shared_ptr<TreeNode>> merged_list = Array<std::shared_ptr<TreeNode>>::merge(list1, list2, cmp);  // O(n+m)
    Array<std::shared_ptr<TreeNode>> merged_list_no_duplicates = removeDuplicates(merged_list);
    return merged_list_no_duplicates;
}

Array<std::shared_ptr<TreeNode>> RankTree::removeDuplicates(const Array<std::shared_ptr<TreeNode>>& list) {
    int real_size = 1;
    int size = list.getSize();
    for(int i = 0; i < size; ++i) {  // O(m+n)
        real_size += list[i]->level_id != list[i+1]->level_id;
    }
    Array<std::shared_ptr<TreeNode>> new_list(real_size);
    for(int i=0; i <= size; ++i) {  // O(m+n)
        if((i < size && (list[i]->level_id != list[i+1]->level_id)) || (i == size && (list[i-1]->level_id != list[i]->level_id))) {
            new_list.push_back(list[i]);
        }
        else {
            list[i]->players_in_level += list[i+1]->players_in_level;
            increase_counts(list[i], list[i+1]);
            increase_counts(list[i]->self_scores_hist, list[i+1]->self_scores_hist, list[i]->scale);
            new_list.push_back(list[i]);
            i++;
        }
    }
    return new_list;
}

void RankTree::resetHistogram(int* histogram, int scale) {  // O(1)
    for (int i = 0; i < scale; i++) {
        histogram[i] = 0;
    }
}

void RankTree::updateMergedTree(std::shared_ptr<TreeNode>& root) {
    if(!root) {
        return;
    }
    updateMergedTree(root->left);
    updateMergedTree(root->right);
    resetHistogram(root->scores_hist, root->scale);
    increase_counts(root->scores_hist, root->self_scores_hist, root->scale);
    increase_counts(root, root->left);
    increase_counts(root, root->right);
    recalculate_average(root);
}

RankTree* RankTree::listToRankTree(const Array<std::shared_ptr<TreeNode>>& list, int scale) {
    RankTree* rt = getTreeFromList(list, scale);  // O(n+m)
    return rt;
    // counting copying and destructing lists and trees (if performed), the overall complexity is still O(n+m)
}
RankTree* RankTree::merge(const RankTree& rt1, const RankTree& rt2) {
    Array<std::shared_ptr<TreeNode>> merged_list = mergeToList(rt1, rt2);
    RankTree* res = listToRankTree(merged_list, rt1.scale);
    updateMergedTree(res->root);
    return res;
}
/********************************** RANK FUNCTIONS **********************************/
int RankTree::findUpperBound(std::shared_ptr<TreeNode>& root, const int level_id) {
    if(!root) {
        return level_id+1;
    }
    if(root->level_id == level_id) {
        return level_id;
    }
    else if(root->level_id < level_id) {
        if(root->right) {
            int tmp = findUpperBound(root->right, level_id);
            if(tmp > root->level_id && tmp <= level_id) {
                return tmp;
            }
        }
        return root->level_id;
    }
    else {  // root->level_id > level_id
        if(root->left) {
            return findUpperBound(root->left, level_id);   
        }
        return root->level_id; // shouldnt happen
    }
}

int RankTree::findLowerBound(std::shared_ptr<TreeNode>& root, const int level_id) {
    if(!root){
        return level_id-1;
    }
    if(root->level_id == level_id) {
        return level_id;
    }
    else if(root->level_id < level_id) {
        if(root->right) {
            return findLowerBound(root->right, level_id);
        }
        return root->level_id; // shouldnt happen
    }
    else {  // root->level_id > level_id
        if(root->left) {
            int tmp = findLowerBound(root->left, level_id);
            if(tmp < root->level_id && tmp >= level_id) {
                return tmp;
            }
        }
        return root->level_id;
    }
}
int RankTree::getSubtreeAtScore(std::shared_ptr<TreeNode>& root, const int score) {
    if(!root || score > MAX_SCORE || score < 0) {
        return 0;
    }
    return root->scores_hist[score];
}
void RankTree::RankAndScoreRankAux(std::shared_ptr<TreeNode>& root, const int bound, const int score, int* const sum_players, int* const sum_score) {
    if(!root) {
        return;
    }
    if(root->level_id < bound) {
        *sum_players += getPlayersInSubtree(root->left) + root->players_in_level;
        *sum_score += getSubtreeAtScore(root, score) - getSubtreeAtScore(root->right, score);
        RankAndScoreRankAux(root->right, bound, score, sum_players, sum_score);
    }
    else if (root->level_id > bound) {
        RankAndScoreRankAux(root->left, bound, score, sum_players, sum_score);
    }
    else {
        *sum_players += getPlayersInSubtree(root->left) + root->players_in_level;
        *sum_score += getSubtreeAtScore(root, score) - getSubtreeAtScore(root->right, score);
    }
}

double RankTree::getPercentOfPlayersWithScoreInBounds(const int lower, const int upper, const int score) {
    update_zero_path(root);
    int closest_lower = findLowerBound(root, lower);
    int closest_upper = findUpperBound(root, upper);
    if(closest_upper < closest_lower || upper < 0 || upper < lower) {
        return -1;
    }
    else if(closest_upper == closest_lower) {
        std::shared_ptr<TreeNode> level = findLevel(closest_upper);
        if(level->players_in_level == 0) {
            return 0;
        }
        return 100 * double(level->self_scores_hist[score]) / level->players_in_level;
    }
    int rank_upper = 0, rank_lower = 0, rank_score_upper = 0, rank_score_lower = 0;
    RankAndScoreRankAux(root, closest_upper, score, &rank_upper, &rank_score_upper);
    RankAndScoreRankAux(root, closest_lower, score, &rank_lower, &rank_score_lower);
    if(closest_lower >= lower || closest_lower == 0) {  // include lower bound in count
        std::shared_ptr<TreeNode> level = findLevel(closest_lower);
        rank_lower -= level->players_in_level;
        rank_score_lower -= level->self_scores_hist[score];
    }
    if(rank_upper - rank_lower == 0) {
        return -1;
    }
    return 100 * double(rank_score_upper - rank_score_lower) / (rank_upper - rank_lower);
}

/********************************** AVERAGE HIGHEST FUNCTIONS **********************************/
double RankTree::averageHighestPlayerLevelByGroupAux(std::shared_ptr<TreeNode>& root, int m) {
    if(!root) {
        return 0;
    }
    else if(root->players_in_subtree == m) {
        return root->average_level_in_subtree;
    }
    if(getPlayersInSubtree(root->right) >= m) {
        return averageHighestPlayerLevelByGroupAux(root->right, m);
    }
    else {
        int remaining = m - getPlayersInSubtree(root->right);
        if(!root->right && root->players_in_level >= m) {
            return root->level_id;
        }
        else if(!root->right || remaining > root->players_in_level) {
            int m2 = remaining - root->players_in_level;
            double sum_levels_r = double(getAverageLevelInSubtree(root->right) * getPlayersInSubtree(root->right) +
                                  root->players_in_level * root->level_id);
            double average_l = averageHighestPlayerLevelByGroupAux(root->left, m2);
            return (sum_levels_r + (average_l * m2)) / m;
        }
        else {
            // can complete to m with players in root
            return double(getAverageLevelInSubtree(root->right) * getPlayersInSubtree(root->right) + remaining * root->level_id) / m;
        }
    }
}

double RankTree::averageHighestPlayerLevelByGroup(int m) {
    return averageHighestPlayerLevelByGroupAux(root, m);
}

/********************************** AVERAGE HIGHEST FUNCTIONS **********************************/

void RankTree::getPlayersBoundsAux(std::shared_ptr<TreeNode>& root, int score, int m, int* lower, int* upper) {
    if(!root) {
        return;
    }
    if(getPlayersInSubtree(root->right) >= m) {
        return getPlayersBoundsAux(root->right, score, m, lower, upper);
    }
    else {
        int remaining = m - getPlayersInSubtree(root->right);
        if(root->players_in_level >= remaining) {
            *lower += max(0, remaining - (root->players_in_level - root->self_scores_hist[score]));
            *upper += min(remaining, root->self_scores_hist[score]);
            if(root->right) {
                *lower += root->right->scores_hist[score];
                *upper += root->right->scores_hist[score];
            }
        }
        else if(remaining > root->players_in_level) {
            int m2 = remaining - root->players_in_level;
            *lower += root->scores_hist[score];
            *upper += root->scores_hist[score];
            if(root->left) {
                *lower -= root->left->scores_hist[score];
                *upper -= root->left->scores_hist[score];
            }
            getPlayersBoundsAux(root->left, score, m2, lower, upper);
        }
    }
}

void RankTree::getPlayersBounds(int score, int m, int* lower, int* upper) {
    getPlayersBoundsAux(root, score, m, lower, upper);
}

/************************************** PUBLIC MEMBER FUNCTUINS **************************************/

RankTree::RankTree(int scale) : scale(scale), root(std::make_shared<TreeNode>(scale, 0)), number_of_levels(1), level_zero(root) { }

const std::shared_ptr<TreeNode>& RankTree::findLevel(const int level_id) const {
    return find_level_aux(root, level_id);
}

void RankTree::insert(std::shared_ptr<Player>& player) {
    if(player->level == 0) {
        insert_player_aux(level_zero, 0, player);
        return;
    }
    if(!findLevel(player->level)) {
        update_zero_path(root);
        insert_level_aux(root, player->level, player, root->scale);
        number_of_levels++;
        return;
    }
    insert_player_aux(root, player->level, player);
}

void RankTree::removePlayer(std::shared_ptr<Player>& player) {
    std::shared_ptr<TreeNode> node = findLevel(player->level);  // O(logn)
    if(!node) {
        return;
    }
    remove_player_aux(root, player);  // O(logn)
    if(node->players_in_level == 0 && node->level_id != 0) {
        update_zero_path(root);
        remove_level_aux(root, player->level);  // O(logn)
        number_of_levels--;
    }
}

int RankTree::getPlayersInTree() {
    update_zero_path(root);
    return root->players_in_subtree;
}

void RankTree::change_player_score(const int level, const int old_score, const int new_score) {
    change_player_score_aux(root, level, old_score, new_score);
}