#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <vector>

class Move
{
    public:
    int rotation;
    int column;
    
    Move();
    Move(int r,int c);
    
    bool operator==(Move &test);
    bool operator!=(Move &test);
};

class Block
{
    public:
    char cell[4][4];
    
    Block();
    Block(std::string);
    
    void print();
    
    bool operator==(Block &test);
};

class Gamestate
{
    public:
    Gamestate();
    Gamestate(int start_level);
    
    void make_move(Move move);
    bool is_valid(Move move);
    std::vector<Move> get_valid_moves();
    void print();
    void clear_board();
    int count_filled_cells();
    
    void add_lines(int lines);
    int score;
    
    int current_block_number;
    int next_block_number;
    int lines[5];
    bool game_over;
    char board[25][10];
    Block blocks[7][4];
    
    int base_lines[20];
    int line_score[5];
    int start_level;
    
    int get_current_level();
    int get_total_lines();
    
    
};

#endif