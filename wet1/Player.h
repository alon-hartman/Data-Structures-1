#ifndef PLAYER_H
#define PLAYER_H

#include <exception>
#include <iostream>
#include "Group.h"

// typedef enum {
//     ByLevel,
//     ByID
// } CompareType;

struct Group;
class Player {
    public:
        int id;
        int level;
        Group* group;
        // CompareType compare_method;
        Player(int id, int level = 1) : 
                id(id), level(level), group(nullptr) { }

        // void changeMethod() {
        //     if(compare_method == ByLevel) {
        //         compare_method = ByID;
        //     }
        //     else {
        //         compare_method = ByLevel;
        //     }
        // }
        // bool operator<(const Player& other) const {
        //     if(compare_method == ByID) {
        //         return this->id < other.id;
        //     }
        //     if(this->level == other.level) {
        //         return this->id < other.id;
        //     }
        //     return level < other.level;
        // }
        // bool operator>(const Player& other) const {
        //     return other < *this;
        // }
        // bool operator==(const Player& other) const {
        //     return id == other.id;
        // }
        // bool operator!=(const Player& other) const {
        //     return !(*this == other);
        // }        
};

#endif

