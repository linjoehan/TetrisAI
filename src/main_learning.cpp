#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <math.h>

#include "tetris_player.h"
#include "coefficient_db.h"

class Player_attributes
{
    public:
    std::vector<double> coefficients;
    int score;
    
    Player_attributes()
    {
        score = 0;
    }
    
    //set coefficients so that max is 1 and others are scales to it
    void scale()
    {
        double max_coefficient = 1;
        for(unsigned i = 0;i<coefficients.size();i++)
        {
            max_coefficient = std::max( max_coefficient ,abs(coefficients[i]));
        }
        for(unsigned i = 0;i<coefficients.size();i++)
        {
            coefficients[i] = coefficients[i] / max_coefficient;
        }
    }
    
    void print()
    {
        std::cout << "Score: " << score;
        std::cout << " Values: ";
        for(unsigned i = 0;i<coefficients.size();i++)
        {
            std::cout << coefficients[i] << " ";
        }
        std::cout << std::endl;
    }
};

bool player_attributes_sorter_greatest(Player_attributes a, Player_attributes b)
{
    return a.score > b.score;
}

const double max_step_value = 0.1; //maximum change in mutation step
const int max_step_ratio = 0.1; //maximum step percentage

int main()
{
    int run_option = 0;
    std::cout << "Please select option of learning method" << std::endl;
    std::cout << "1 - Normal mode - Runs in jnes with population size 10, takes long to complete a game" << std::endl;
    std::cout << "2 - Fast mode - Runs in internal simulator with population size 100, faster learning with slightly less accuracy" << std::endl;
    while(not(run_option==1 or run_option==2))
    {
        std::cout << "Option:";
        std::cin >> run_option;
        std::cin.ignore();
    }
    
    unsigned population_size = (run_option==1 ? 10 : 100);
    
    int generation = 0;
    //Get population of 10 player attributes by randomly generating them or getting the latest one from the database
    std::vector<Player_attributes> player_attributes(population_size);
    //fill player attributes from database
    {
        Coefficient_db db_object;
        std::vector<std::vector<double>> read_in;
        read_in = (run_option == 1 ? db_object.get_learning() : db_object.get_learning_fast());
        if(read_in.size() != population_size)
        {
            std::cerr << "Size of read in coefficients don't match the size of the expected population exiting..." << std::endl;
            return -1;
        }
        
        for(unsigned i =0;i<population_size;i++)
        {
            player_attributes[i].coefficients = read_in[i];
        }
    }
    
    Tetris_Player player;
    if(run_option==1)
    {
        player.init();
    }
    
    bool running = true;
    
    while(running)
    {
        generation++;
        std::cout << "Generation: " << generation <<std::endl;
        std::cout << "Generation: " << generation << " " << "Time:" << (clock() / CLOCKS_PER_SEC) << std::endl;
        //write to DB the current values of player attributes
        {
            std::vector<std::vector<double>> write_data;
            for(unsigned i = 0;i<player_attributes.size();i++)
            {
                write_data.push_back(player_attributes[i].coefficients);
            }
            Coefficient_db db_object;
            
            if(run_option==1)
            {
                db_object.write_learning(write_data);
            }
            else if(run_option == 2)
            {
                db_object.write_learning_fast(write_data);
            }
        }
        
        //Apply next generation to player attributes
        {
            std::vector<Player_attributes> next_generation;
            //add the 20% best
            int carry_over = 20 * population_size / 100;
            for(int i = 0;i<carry_over;i++)
            {
                next_generation.push_back(player_attributes[i]);
                next_generation[i].score = 0;
            }
            //build the next 60% by mutating the the first 20%
            for(int base = 0;base<carry_over;base++)
            {
                for(int i = 0;i<3;i++)
                {
                    std::vector<double> mutate;
                    for(unsigned j =0;j<player_attributes[base].coefficients.size();j++)
                    {
                        mutate.push_back( player_attributes[base].coefficients[j] + std::max(max_step_value , player_attributes[base].coefficients[j] * max_step_ratio)  * (rand() * 2.0 / RAND_MAX - 1.0) );
                    }
                    Player_attributes next_player;
                    next_player.coefficients = mutate;
                    next_generation.push_back(next_player);
                }
            }
            
            //build next 20% from crossover between the first 20% randomly
            while(next_generation.size() < population_size)
            {
                std::vector<double> cross_over;
                for(unsigned j =0;j<player_attributes[0].coefficients.size();j++)
                {
                    cross_over.push_back(player_attributes[rand()%carry_over].coefficients[j]);
                }
                Player_attributes next_player;
                next_player.coefficients = cross_over;
                next_generation.push_back(next_player);
            }
            
            player_attributes = next_generation;
        }
        
        //Run games to get scores
        for(unsigned player_number = 0; player_number < population_size ; player_number++)
        {
            std::cout << "Game: " << player_number + 1 << " of " << population_size << std::endl;
            
            if(run_option == 1)
            {
                if(player.player_active)
                {
                    player.gamestate_ai.update_coefficients(player_attributes[player_number].coefficients);
                    player.play(18);
                    if(player.player_active)
                    {
                        player_attributes[player_number].score = player.gamestate.score;
                    }
                }
            }
            else if(run_option == 2)
            {
                for(int game_number = 0;game_number<1;game_number++)
                {
                    Gamestate_ai gamestate_ai;
                    gamestate_ai.update_coefficients(player_attributes[player_number].coefficients);
                    Gamestate gamestate = Gamestate(18);
                    
                    bool game_over = false;
                    while(game_over == false)
                    {
                        Move move = gamestate_ai.get_best_move(gamestate);
                        gamestate.make_move(move);
                        
                        //check gamestate level
                        if(gamestate.get_current_level() >28)
                        {
                            game_over = true;
                        }
                        
                        //check gamestate height
                        {
                            int max_height = 0;
                            for(int row = 0;row<25;row++)
                            {
                                for(int col = 0;col<10;col++)
                                {
                                    if(gamestate.board[row][col] == '#')
                                    {
                                        max_height = std::max(max_height, 24 - row);
                                    }
                                }
                            }
                            
                            if(max_height > 12)
                            {
                                game_over = true;
                            }
                        }
                    }
                    
                    player_attributes[player_number].score = std::max(player_attributes[player_number].score,gamestate.score);
                    std::cout << (game_number==0 ? "":" ") << gamestate.score;
                }
                std::cout << std::endl;
            }
            player_attributes[player_number].print();
        }
        
        running = player.player_active;
        
        //sort player attributes
        std::sort(player_attributes.begin(),player_attributes.end(),player_attributes_sorter_greatest);
        
        //print results as a debug
        std::cout << "Final results:" << std::endl;
        for(int i = 0;i<10;i++)
        {
            player_attributes[i].print();
        }
        
        if(running)
        {
            //write some data to file
            std::fstream data_file;
            if(run_option==1)
            {
                data_file.open("../coefficient_db/learning_data.txt",std::ios::out | std::ios::app);
            }
            else if(run_option==2)
            {
                data_file.open("../coefficient_db/learning_fast_data.txt",std::ios::out | std::ios::app);
            }
            
            if(data_file.is_open())
            {
                for(int i = 0;i<10;i++)
                {
                    data_file << (i==0?"":",")<<player_attributes[i].score;
                }
                for(unsigned i = 0;i<player_attributes[0].coefficients.size();i++)
                {
                    data_file << "," << player_attributes[0].coefficients[i];
                }
                data_file << std::endl;
            }
            data_file.close();
        }
    }
    
    std::cin.get();
}