#ifndef PLAYER_H
#define PLAYER_H

#include <exception>
#include <iostream>

typedef enum {
    ByLevel,
    ByID
} CompareType;

class Player {
    private:
        int id;
        int level;
        CompareType compare_method;
    public:
        Player(int id, int level = 1, CompareType cmp = ByID) : 
                id(id), level(level), compare_method(cmp) { }
        ~Player() = default;
        Player(const Player& other)
        {
            this->id = other.id;
            this->level = other.level;
            this->compare_method = other.compare_method;
        }
        void changeMethod() {
            if(compare_method == ByLevel) {
                compare_method = ByID;
            }
            else {
                compare_method = ByLevel;
            }
        }
        bool operator<(const Player& other) const {
            if(compare_method == ByID) {
                return this->id < other.id;
            }
            if(this->level == other.level) {
                return this->id < other.id;
            }
            return level < other.level;
        }
        bool operator>(const Player& other) const {
            return other < *this;
        }
        bool operator==(const Player& other) const {
            return id == other.id;
        }
        bool operator!=(const Player& other) const {
            return !(*this == other);
        }
        
};
#endif

