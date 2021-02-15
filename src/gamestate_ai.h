#ifndef GAMESTATE_AI_H
#define GAMESTATE_AI_H

#include <vector>
#include "gamestate.h"

class Gamestate_ai
{
    public:
    Gamestate_ai();
    
    Move get_best_move(Gamestate gamestate);
    
    private:
    void dfs_search(Gamestate gamestate, int depth, std::vector<Move> moves, Move &best_move,double &best_score);
    double eval(Gamestate gamestate);
};

#endif