#include "tetris_player.h"

int main()
{
    Tetris_Player player;
    player.init();
    
    while(player.player_active)
    {
        player.play(18);
    }
    
    std::cin.get();
}