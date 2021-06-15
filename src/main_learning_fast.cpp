#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <time.h>

#include "gamestate.h"
#include "gamestate_ai.h"
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
        double max_coefficient = 0;
        for(unsigned i = 0;i<coefficients.size();i++)
        {
            max_coefficient = std::max(max_coefficient,coefficients[i]);
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

const int population_size = 100;
const int max_step_value = 20; //percentage of maximum change in mutation step

int main()
{
    int generation = 0;
    //Get population of 10 player attributes by randomly generating them or getting the latest one from the database
    std::vector<Player_attributes> player_attributes(population_size);
    //fill player attributes from database
    {
        Coefficient_db db_object;
        std::vector<std::vector<double>> read_in = db_object.get_learning_fast();
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
    
    //Tetris_Player player;
    //player.init();
    
    while(true)
    {
        generation++;
        std::cout << "Generation: " << generation << " " << "Time:" << (clock() / CLOCKS_PER_SEC) << std::endl;
        //write to DB the current values of player attributes
        {
            std::vector<std::vector<double>> write_data;
            for(unsigned i = 0;i<player_attributes.size();i++)
            {
                write_data.push_back(player_attributes[i].coefficients);
                Coefficient_db db_object;
                db_object.write_learning_fast(write_data);
            }
        }
        
        //Apply next generation to player attributes
        {
            std::vector<Player_attributes> next_generation;
            //add the 20 best
            for(int i = 0;i<20;i++)
            {
                next_generation.push_back(player_attributes[i]);
                next_generation[i].score = 0;
            }
            //build the next 60 by mutating the the first 20
            for(int base = 0;base<20;base++)
            {
                for(int i = 0;i<3;i++)
                {
                    std::vector<double> mutate;
                    for(int j =0;j<player_attributes[base].coefficients.size();j++)
                    {
                        mutate.push_back( player_attributes[base].coefficients[j] + player_attributes[base].coefficients[j] * rand() / RAND_MAX * max_step_value / 100);
                    }
                    Player_attributes next_player;
                    next_player.coefficients = mutate;
                    next_generation.push_back(next_player);
                }
            }
            
            //build next 20 from crossover between the first 20 randomly
            for(int i = 0;i<20;i++)
            {
                std::vector<double> mutate;
                for(int j =0;j<player_attributes[0].coefficients.size();j++)
                {
                    mutate.push_back(player_attributes[rand()%20].coefficients[j]);
                }
                Player_attributes next_player;
                next_player.coefficients = mutate;
                next_generation.push_back(next_player);
            }
            
            //scale the next generation so that the max coefficient value is 1
            for(unsigned i =0;i<next_generation.size();i++)
            {
                next_generation[i].scale();
            }
            
            player_attributes = next_generation;
        }
        
        //Run games to get scores
        for(unsigned player_number = 0; player_number < population_size ; player_number++)
        {
            std::cout << "Game: " << player_number + 1 << " of " << population_size << std::endl;
            
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
                        int height[10] = {0};
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
                        
                        if(max_height >= 10)
                        {
                            game_over = true;
                        }
                    }
                }
                
                player_attributes[player_number].score = std::max(player_attributes[player_number].score,gamestate.score);
                std::cout << (game_number==0 ? "":" ") << gamestate.score;
            }
            std::cout << std::endl;
            
            player_attributes[player_number].print();
        }
            
        //sort player attributes
        std::sort(player_attributes.begin(),player_attributes.end(),player_attributes_sorter_greatest);
        
        //print results as a debug
        std::cout << "Final results:" << std::endl;
        for(int i = 0;i<10;i++)
        {
            player_attributes[i].print();
        }
        
        //write some data to file
        std::fstream data_file;
        data_file.open("../coefficient_db/learning_fast_data.txt",std::ios::out | std::ios::app);
        if(data_file.is_open())
        {
            for(int i = 0;i<10;i++)
            {
                data_file << (i==0?"":",")<<player_attributes[i].score;
            }
            data_file << std::endl;
        }
        data_file.close();
    }
    
    std::cin.get();
}