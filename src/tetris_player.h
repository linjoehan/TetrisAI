#ifndef TETRIS_PLAYER_H
#define TETRIS_PLAYER_H

#include <iostream>
#include <time.h> 

#include "jnes_interface.h"
#include "gamestate.h"
#include "gamestate_ai.h"

class Pixel
{
    public:
    uint8_t r,g,b,i;
    
    Pixel();
};

class Tetris_Player
{
    public:
    bool player_active;
    bool action_required;
    
    //Used to keep track of the current games score and can be used to report back after each game ends
    int base_lines[20]; //tetris rules for when you level up based on the level you start
    int line_score[5]; //base scoreing for number of lines cleared in one move
    int current_game_lines;
    int current_game_score;
    
    Jnes_Interface jnes_interface;
    Gamestate gamestate;
    Gamestate_ai gamestate_ai;
    
    Tetris_Player();
    void init();
    void get_gamestate();
    void play(int level);
    
    //private:
    int start_level;
    bool game_over;
    int starting_column[7][4];
    
    Gamestate prev_gamestate;
    
    void press_button(uint8_t a);
    void fill_state_from_image();
    void fill_next_piece();
    void search_current_block();
    void check_game_over();
    Pixel get_pixel(int row,int col);
    
    void start_game();
    
    bool high_score_screen();
    bool victory_screen();
    bool game_over_screen();
    void reset_to_start();
    
    void update_current_game_score();
};



#endif