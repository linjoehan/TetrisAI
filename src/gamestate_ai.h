#ifndef GAMESTATE_AI_H
#define GAMESTATE_AI_H

#include <vector>
#include "gamestate.h"
#include "coefficient_db.h"

class Gamestate_ai
{
    public:
    Gamestate_ai();
    Move get_best_move(Gamestate gamestate);
    void update_coefficients(std::vector<double> copy);
    
    private:
    std::vector<double> coefficients;
    Coefficient_db coefficient_db;
    
    
    void dfs_search(Gamestate gamestate, int depth, std::vector<Move> moves, Move &best_move,double &best_score);
    double eval(Gamestate gamestate);
};

#endif