#include "gamestate_ai.h"

Gamestate_ai::Gamestate_ai()
{
    coefficients = coefficient_db.get_default();
}

Move Gamestate_ai::get_best_move(Gamestate gamestate)
{
    Move best_move = Move(0,0);
    double best_score = -1000000000;
    
    dfs_search(gamestate,2,{},best_move,best_score);
    return best_move;
}

void Gamestate_ai::update_coefficients(std::vector<double> copy)
{
    //Warning messages when copy has the wrong size
    if(copy.size() < coefficient_db.coefficients_size)
    {
        std::cerr << "WARN: Coefficients vector received is to small: received a vector of size " << copy.size() << " expected " << coefficient_db.coefficients_size << " filling to required size with zeros" << std::endl;
    }
    if(copy.size() > coefficient_db.coefficients_size)
    {
        std::cerr << "WARN: Coefficients vector received is to large: received a vector of size " << copy.size() << " expected " << coefficient_db.coefficients_size << " ignoring the extra values received" << std::endl;
    }
    
    coefficients.clear();
    for(unsigned i = 0;i < std::min(copy.size(),coefficient_db.coefficients_size);i++)
    {
        coefficients.push_back(copy[i]);
    }
    while(coefficients.size()<coefficient_db.coefficients_size)
    {
        coefficients.push_back(0);
    }
}

void Gamestate_ai::dfs_search(Gamestate gamestate, int depth, std::vector<Move> moves, Move &best_move,double &best_score)
{
    if(depth == 0)
    {
        double test_score = eval(gamestate);
        if(test_score > best_score)
        {
            best_move = moves[0];
            best_score = test_score;
        }
        return;
    }
    
    if(gamestate.game_over)
    {
        return;
    }
    
    std::vector<Move> possible_moves = gamestate.get_valid_moves();
    for(unsigned i = 0;i<possible_moves.size();i++)
    {
        Gamestate copystate = gamestate;
        std::vector<Move> copymoves = moves;
        
        copystate.make_move(possible_moves[i]);
        copymoves.push_back(possible_moves[i]);
        
        dfs_search(copystate,depth-1,copymoves,best_move,best_score);
    }
}

double Gamestate_ai::eval(Gamestate gamestate)
{
    //get height for each column
    int height[10] = {0};
    for(int row = 0;row<25;row++)
    {
        for(int col = 0;col<10;col++)
        {
            if(gamestate.board[row][col] == '#')
            {
                height[col] = std::max(height[col] , 24 - row);
            }
        }
    }
    
    int rightlane_height = height[9];
    int max_height = 0;
    int aggregate_height = 0;
    for(int i = 0;i<10;i++)
    {
        aggregate_height += height[i];
        max_height = std::max(max_height , height[i]);
    }
    
    int lines_single = gamestate.lines[1];
    int lines_double = gamestate.lines[2];
    int lines_triple = gamestate.lines[3];
    int lines_tetris = gamestate.lines[4];
    
    int holes = 0;
    for(int row = 0;row<25;row++)
    {
        for(int col = 0;col<10;col++)
        {
            if(gamestate.board[row][col] == ' ')
            {
                int test_height = 24 - row;
                if(height[col] > test_height)
                {
                    holes++;
                }
            }
        }
    }
    
    int blocks_above_holes = 0;
    for(int col = 0;col<10;col++)
    {
        int blocks_counted = 0;
        for(int row = 0;row<25;row++)
        {
            if(gamestate.board[row][col] == '#')
            {
                blocks_counted++;
            }
            else
            {
                blocks_above_holes += blocks_counted;
            }
        }
    }
    
    int bumpiness = 0;
    for(int i = 1;i<10;i++)
    {
        bumpiness += abs(height[i] - height[i-1]);
    }
    
    double score = 0
                 - coefficients[0] * max_height
                 - coefficients[1] * aggregate_height
                 - coefficients[2] * rightlane_height
                 - coefficients[3] * bumpiness
                 - coefficients[4] * holes
                 - coefficients[5] * blocks_above_holes
                 - coefficients[6] * lines_single
                 - coefficients[7] * lines_double
                 - coefficients[8] * lines_triple
                 + coefficients[9] * lines_tetris;
                 
    return score;
    
}