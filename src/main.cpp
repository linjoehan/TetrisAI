#include <iostream>
#include <fstream>

//#include "gamestate.h"
//#include "gamestate_ai.h"
//#include "jnes_interface.h"
#include "tetris_player.h"


int main()
{
    Tetris_Player player;
    player.init();
    
    while(player.player_active)
    {
        player.start_level_eighteen();
        //player.start_level_zero();
        player.play();
    }
    
    std::cin.get();
}

