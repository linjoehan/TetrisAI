#include "tetris_player.h"

Pixel::Pixel()
{
    r=0;
    g=0;
    b=0;
    i=0;
}

Tetris_Player::Tetris_Player()
: jnes_interface("../emu/Jnes/","../emu/Tetris (U) [!].zip"),
  player_active(true),
  action_required(false),
  starting_column{{3,5,3,5},{4,4,4,4},{4,4,4,5},{4,4,4,5},{4,4,4,5},{4,5,4,4},{4,5,4,4}}
{
}

void Tetris_Player::init()
{
    jnes_interface.init();
    Sleep(15000);
}

void Tetris_Player::play()
{
    while(player_active)
    {
        get_gamestate();
        if(action_required)
        {
            Move move = gamestate_ai.get_best_move(gamestate);
            switch(move.rotation)
            {
                case 2: press_button(0b00000010);
                case 1: press_button(0b00000010);
                        break;
                case 3: press_button(0b00000001);
                        break;
            }
            
            int current_column = starting_column[gamestate.current_block_number][move.rotation];
            
            while(current_column != move.column)
            {
                if(current_column < move.column)
                {
                    press_button(0b00010000);
                    current_column++;
                }
                else
                {
                    press_button(0b00100000);
                    current_column--;
                }
            }
            
            //Gamestate teststate = gamestate;
            //teststate.make_move(move);
            //teststate.print();
        }
    }
}

void Tetris_Player::start_level_zero()
{
    //Start
    press_button(0b00000100);
    Sleep(100);
    
    //no music
    press_button(0b01000000);
    press_button(0b01000000);
    press_button(0b01000000);

    //Start
    press_button(0b00000100);
    Sleep(100);
    
    //Start
    press_button(0b00000100);
}

void Tetris_Player::start_level_eighteen()
{
    //Start
    press_button(0b00000100);

    //no music
    press_button(0b01000000);
    press_button(0b01000000);
    press_button(0b01000000);

    //Start
    press_button(0b00000100);
    
    //select level
    press_button(0b00010000);
    press_button(0b00010000);
    press_button(0b00010000);
    press_button(0b01000000);
    
    //Start with +10
    jnes_interface.update_joystick(0b00000010);
    jnes_interface.update_joystick(0b00000110);
    jnes_interface.update_joystick(0b00000000);
}

void Tetris_Player::press_button(uint8_t a)
{
    jnes_interface.update_joystick(a);
    jnes_interface.update_joystick(0);
}

void Tetris_Player::get_gamestate()
{
    if(jnes_interface.update_screen_data() != 0)
    {
        player_active = false;
        return;
    }
    
    //fill game state from screen data
    fill_state_from_image();
    
    //get next piece from screen data
    fill_next_piece();
    
    //search and lift current piece from state if possible and require action from ai
    search_current_block();
    
    //check end game over state
    check_game_over();
}

void Tetris_Player::fill_state_from_image()
{
    gamestate.clear_board();
    
    for(int row = 0;row<20;row++)
    {
        for(int col = 0;col<10;col++)
        {
            Pixel test = get_pixel(43+row*8,99+col*8);
            if(test.r + test.g + test.b > 10)
            {
                gamestate.board[row+5][col] = '#';
            }
        }
    }
}

void Tetris_Player::fill_next_piece()
{
    Block test_block("                ");
    for(int row = 2;row<4;row++)
    {
        for(int col = 0;col<3;col++)
        {
            Pixel test = get_pixel(99+row*8,199+col*8);
            if(test.r + test.g + test.b > 10)
            {
                test_block.cell[row][col] = '#';
            }
        }
    }
    
    int res_block = 0;
    for(int i = 0;i<7;i++)
    {
        if(test_block == gamestate.blocks[i][0])
        {
            res_block = i;
            break;
        }
    }
    
    gamestate.next_block_number = res_block;
}

void Tetris_Player::search_current_block()
{
    action_required = false;
    
    for(int search_col = 3;search_col<=4;search_col++)
    {
        for(int search_row = 0;search_row<=4;search_row++)
        {
            //check that board below block is clear
            bool clear_below = true;
            for(int clear_test_col = 0;clear_test_col<4;clear_test_col++)
            {
                if(gamestate.board[search_row + 4][search_col + clear_test_col] == '#')
                {
                    clear_below = false;
                }
            }
            
            if(clear_below)
            {
                Block test_block("                ");
                for(int row = 0;row<4;row++)
                {
                    for(int col = 0;col<4;col++)
                    {
                        test_block.cell[row][col] = gamestate.board[search_row+row][search_col+col];
                    }
                }
                
                for(int block_number = 0;block_number<7;block_number++)
                {
                    if( (block_number == 0 and search_col==3) or (block_number>0 and search_col==4) )
                    {
                        if(test_block == gamestate.blocks[block_number][0])
                        {
                            gamestate.current_block_number = block_number;
                            //remove block from board
                            for(int row = 0;row<4;row++)
                            {
                                for(int col = 0;col<4;col++)
                                {
                                    gamestate.board[search_row+row][search_col+col] = ' ';
                                }
                            }
                            action_required = true;
                            return;
                        }
                    }
                }
            }
        }
    }
}

void Tetris_Player::check_game_over()
{
    bool game_over = true;
    for(int col = 0;col<10 and game_over;col++)
    {
        if(gamestate.board[5][col] == ' ')
        {
            game_over = false;
        }
    }
    
    if(game_over)
    {
        player_active = false;
    }
}

Pixel Tetris_Player::get_pixel(int row,int col)
{
    Pixel res;
    unsigned offset = (row*262 + col) * 4;
    res.r = *(jnes_interface.screendata + offset);
    res.g = *(jnes_interface.screendata + offset + 1);
    res.b = *(jnes_interface.screendata + offset + 2);
    res.i = *(jnes_interface.screendata + offset + 3);
    return res;
}