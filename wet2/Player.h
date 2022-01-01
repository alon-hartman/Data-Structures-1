#ifndef PLAYER_H
#define PLAYER_H

struct Player {
    int playerID;
    int groupID;
    int score;
    int level;

    Player(int playerID=0, int groupID=0, int score=0, int level=0) : 
        playerID(playerID), groupID(groupID), score(score), level(level) { }
};

#endif