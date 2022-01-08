#ifndef LEVEL_H
#define LEVEL_H 
#include "RankTree.h"
#include <memory>

struct Group {
    std::shared_ptr<RankTree> rt;
    int groupID;
    int number_of_players;

    Group() : rt(nullptr), groupID(0), number_of_players(0) { }
    Group(int groupID, int scale) : rt(std::make_shared<RankTree>(scale)), groupID(groupID), number_of_players(0) { }

    ~Group() {
        rt.reset();
    }

    void merge(const Group& other) {
        // rt = std::shared_ptr<RankTree>(&RankTree::merge(*rt, *other.rt));
        const RankTree& test = *rt;
        rt = std::shared_ptr<RankTree>(RankTree::merge(test, *other.rt));
        number_of_players += other.number_of_players;
    }
    

};


#endif