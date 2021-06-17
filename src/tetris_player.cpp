#include "tetris_player.h"

Pixel::Pixel()
{
    r=0;
    g=0;
    b=0;
    i=0;
}

Tetris_Player::Tetris_Player()
: player_active(true),
  action_required(false),
  jnes_interface("../emu/Jnes/","../emu/Tetris (U) [!].zip"),
  game_over(true),
  starting_column{{3,5,3,5},{4,4,4,4},{4,4,4,5},{4,4,4,5},{4,4,4,5},{4,5,4,4},{4,5,4,4}}
{
}

void Tetris_Player::init() //gets the game to the level select screen
{
    jnes_interface.init();
    Sleep(15000); //Sleep while game starts up
    
    //Start
    press_button(0b00000100);

    //no music
    press_button(0b01000000);
    press_button(0b01000000);
    press_button(0b01000000);

    //Start
    press_button(0b00000100);
}

void Tetris_Player::play(int level)
{
    //Check that level is a level and default it if needed
    if(level < 0 or level > 19)
    {
        std::cerr << "Starting level cannot be set to " << level << " setting level to 0" << std::endl;
        level = 0;
    }
    
    start_level = level;
    start_game();
    
    while(player_active and !game_over)
    {
        get_gamestate();
        if(action_required)
        {
            update_current_game_score();
            
            Move move = gamestate_ai.get_best_move(gamestate);
            Move current_move = Move(0,starting_column[gamestate.current_block_number][move.rotation]);
            
            while(move != current_move)
            {
                uint8_t required_state = 0;
                
                /*add in required rotation based on this
                required rotation   0 1 2 3
                Current rotation  0 X A A B
                                  1   X A
                                  2     X
                                  3       X
                Other combinations will not come up */
                
                switch(current_move.rotation*4 + move.rotation)
                {
                   case 1:
                   case 2:
                   case 6:
                   {
                       required_state |= 0b00000010;
                       current_move.rotation++;
                   }break;
                   case 3:
                   {
                       required_state |= 0b00000001;
                       current_move.rotation += 3;
                   }break;
                }
                
                //now the move direction
                if(current_move.column < move.column)
                {
                    required_state |= 0b00010000;
                    current_move.column++;
                }
                else if(current_move.column > move.column)
                {
                    required_state |= 0b00100000;
                    current_move.column--;
                }
                
                //update the controller state
                jnes_interface.update_joystick(required_state);
                jnes_interface.update_joystick(0);
            }
        }
    }
    
    if(player_active)
    {
        Sleep(20000);
        game_over_screen();
        Sleep(500);
        victory_screen();
        Sleep(500);
        high_score_screen();
        Sleep(500);
        
        reset_to_start();
    }
}

void Tetris_Player::start_game()
{
    for(int i = 0;i<start_level%10;i++)
    {
        //move up one level in menu
        press_button(0b00010000);
    }
    
    if(start_level > 10)
    {
        //Start with +10
        jnes_interface.update_joystick(0b00000010);
        jnes_interface.update_joystick(0b00000110);
        jnes_interface.update_joystick(0b00000000);
    }
    else
    {
        //Start
        press_button(0b00000100);
    }
    
    //set game over state to false
    gamestate = Gamestate(start_level);
    prev_gamestate = gamestate;
    game_over = false;
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

void Tetris_Player::check_game_over() //uses it's own method to check onscreen for game over status and does not depend on the gameover status in the gamestate object
{
    bool is_game_over = true;
    for(int col = 0;col<10 and is_game_over;col++)
    {
        if(gamestate.board[5][col] == ' ')
        {
            is_game_over = false;
        }
    }
    
    if(is_game_over)
    {
        game_over = is_game_over;
    }
}

Pixel Tetris_Player::get_pixel(int row,int col)
{
    Pixel res;
    unsigned offset = (row*262 + col) * 4;
    res.b = *(jnes_interface.screendata + offset);
    res.g = *(jnes_interface.screendata + offset + 1);
    res.r = *(jnes_interface.screendata + offset + 2);
    res.i = *(jnes_interface.screendata + offset + 3);
    return res;
}

//Filling in the name on the high score screen is a challenge
bool Tetris_Player::high_score_screen()
{
    if(jnes_interface.update_screen_data() != 0)
    {
        player_active = false;
        return false;
    }
    
    Pixel test_high_score_screen = get_pixel(25,228);
    
    if(test_high_score_screen.b > 200 and test_high_score_screen.g > 150)
    {
        /*
        std::string name = "JONAS";
        for(int pos = 0;pos < std::min(int(name.size()) , 6);pos++)
        {
            for(int letter = 0;letter <= name[pos] - 'A';letter++)
            {
                //Up
                jnes_interface.update_joystick(0b10000000);
                Sleep(75); //Searching fo right number here 75 to 80
                jnes_interface.update_joystick(0b00000000);
                Sleep(200);
            }
            //Right
            press_button(0b00010000);
            Sleep(200);
        }
        */
        
        //Trying a different method to fill in the name
        jnes_interface.update_joystick(0b10000000);
        Sleep(1250);
        jnes_interface.update_joystick(0b00000000);
        Sleep(200);
        press_button(0b00010000);
        
        jnes_interface.update_joystick(0b10000000);
        Sleep(1950);
        jnes_interface.update_joystick(0b00000000);
        Sleep(200);
        press_button(0b00010000);
        
        jnes_interface.update_joystick(0b10000000);
        Sleep(1800);
        jnes_interface.update_joystick(0b00000000);
        Sleep(200);
        press_button(0b00010000);
        
        jnes_interface.update_joystick(0b10000000);
        Sleep(70);
        jnes_interface.update_joystick(0b00000000);
        Sleep(200);
        press_button(0b00010000);
        
        jnes_interface.update_joystick(0b10000000);
        Sleep(2450);
        jnes_interface.update_joystick(0b00000000);
        Sleep(200);
        press_button(0b00010000);
        
        //Start 
        press_button(0b00000100);
        Sleep(200);
        
        return true;
    }
    else
    {
        return false;
    }
}

bool Tetris_Player::victory_screen()
{
    if(jnes_interface.update_screen_data() != 0)
    {
        player_active = false;
        return false;
    }
    
    Pixel test_victory_screen = get_pixel(25,228);
    
    //test if this is a victory screen
    if(test_victory_screen.b > 200 and test_victory_screen.g < 150)
    {
        //Start
        press_button(0b00000100);
        Sleep(200);
        return true;
    }
    else
    {
        return false;
    }
}

bool Tetris_Player::game_over_screen()
{
    if(jnes_interface.update_screen_data() != 0)
    {
        player_active = false;
        return false;
    }
    
    Pixel test_game_over_screen = get_pixel(25,228);
    
    //test if this is a victory screen
    if(test_game_over_screen.r < 100 and test_game_over_screen.g < 100 and test_game_over_screen.b < 100)
    {
        //Start
        press_button(0b00000100);
        Sleep(200);
        return true;
    }
    else
    {
        return false;
    }
}

void Tetris_Player::reset_to_start()
{
    //Press up and left to get the selector back to 0
    press_button(0b10000000);
    press_button(0b10000000);
    
    press_button(0b00100000);
    press_button(0b00100000);
    press_button(0b00100000);
    press_button(0b00100000);
    press_button(0b00100000);
    
    
    Sleep(500);
}

void Tetris_Player::update_current_game_score()
{
    //count_filled between current and previous game state
    int prev_filled = prev_gamestate.count_filled_cells();
    int current_filled = gamestate.count_filled_cells();
    
    int lines_added = (prev_filled + 4 - current_filled) / 10;
    gamestate.add_lines(lines_added);
              
    prev_gamestate = gamestate;
}