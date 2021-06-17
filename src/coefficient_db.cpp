#include "coefficient_db.h"

Coefficient_db::Coefficient_db()
:
  coefficients_size(23),
  learning_population(10),
  learning_fast_population(100)
{
    
}

std::vector<double> Coefficient_db::get_default()
{
    std::vector<double> res;
    std::fstream default_file;
    default_file.open("../coefficient_db/default.txt",std::ios::in);
    
    if(default_file.is_open())
    {
        std::string line;
        getline(default_file,line);
        std::vector<std::string> string_default = string_split(line);
        
        //warnings for when size of inputs and expected are different
        if(string_default.size() < coefficients_size)
        {
            std::cerr << "WARN: Coefficients vector received is to small: received a vector of size " << string_default.size() << " expected " << coefficients_size << " filling to required size with ones" << std::endl;
        }
        if(string_default.size() > coefficients_size)
        {
            std::cerr << "WARN: Coefficients vector received is to large: received a vector of size " << string_default.size() << " expected " << coefficients_size << " ignoring the extra values received" << std::endl;
        }
        
        for(unsigned i = 0; i < std::min(string_default.size() ,coefficients_size ); i++)
        {
            res.push_back( std::stod(string_default[i]));
        }
    }
    
    //fill result vector with zeros if required
    while(res.size()<coefficients_size)
    {
        res.push_back(0);
    }
    
    default_file.close();
    
    return res;
}

void Coefficient_db::write_default(std::vector<double> coefficient_value)
{
    std::fstream default_file;
    default_file.open("../coefficient_db/default.txt",std::ios::out);
    
    if(default_file.is_open())
    {
        for(unsigned i = 0;i<coefficient_value.size();i++)
        {
            default_file << (i>0? " ":"") << std::fixed << std::setprecision(15) << coefficient_value[i];
        }
    }
    else
    {
        std::cerr << "WARN: Could not open default file for writing, skipping write" << std::endl; 
    }
    
    default_file.close();
}

std::vector<std::vector<double>> Coefficient_db::get_learning()
{
    std::vector<std::vector<double>> res;
    std::fstream learning_file;
    learning_file.open("../coefficient_db/learning.txt",std::ios::in);
    
    if(learning_file.is_open())
    {
        std::string line;
        while(learning_file.good() and res.size() < learning_population and getline(learning_file,line))
        {
            std::vector<double> res_part;
            std::vector<std::string> input_string = string_split(line);
            
            //warnings for when size of inputs and expected are different
            if(input_string.size() < coefficients_size)
            {
                std::cerr << "WARN: Coefficients vector received for line number "<< res.size()+1 << " is to small: received a vector of size " << input_string.size() << " expected " << coefficients_size << " filling to required size with ones" << std::endl;
            }
            if(input_string.size() > coefficients_size)
            {
                std::cerr << "WARN: Coefficients vector received for line number "<< res.size()+1 << " is to large: received a vector of size " << input_string.size() << " expected " << coefficients_size << " ignoring the extra values received" << std::endl;
            }
            
            for(unsigned i = 0; i < std::min(input_string.size() ,coefficients_size ); i++)
            {
                res_part.push_back( std::stod(input_string[i]));
            }
            
            while(res_part.size() < coefficients_size)
            {
                res_part.push_back(0);
            }
            
            res.push_back(res_part);
        }
    }
    
    while(res.size() < learning_population)
    {
        //push ones to fill to required values
        std::vector<double> res_part;
        while(res_part.size() < coefficients_size)
        {
            res_part.push_back(0);
        }
        res.push_back(res_part);
    }
    
    learning_file.close();
    
    return res;
}

void Coefficient_db::write_learning(std::vector<std::vector<double>> coefficient_value_table)
{
    std::fstream learning_file;
    learning_file.open("../coefficient_db/learning.txt",std::ios::out);
    
    if(learning_file.is_open())
    {
        for(int row = 0;row<coefficient_value_table.size();row++)
        {
            for(int col = 0;col < coefficient_value_table[row].size();col++)
            {
                learning_file << (col>0? " ":"") << std::fixed << std::setprecision(15) << coefficient_value_table[row][col];
            }
            learning_file << std::endl;
        }
    }
    else
    {
        std::cerr << "WARN: Could not open learning file for writing, skipping write" << std::endl; 
    }
    
    learning_file.close();
}


std::vector<std::vector<double>> Coefficient_db::get_learning_fast()
{
    std::vector<std::vector<double>> res;
    std::fstream learning_file;
    learning_file.open("../coefficient_db/learning_fast.txt",std::ios::in);
    
    if(learning_file.is_open())
    {
        std::string line;
        while(learning_file.good() and res.size() < learning_fast_population and getline(learning_file,line))
        {
            std::vector<double> res_part;
            std::vector<std::string> input_string = string_split(line);
            
            //warnings for when size of inputs and expected are different
            if(input_string.size() < coefficients_size)
            {
                std::cerr << "WARN: Coefficients vector received for line number "<< res.size()+1 << " is to small: received a vector of size " << input_string.size() << " expected " << coefficients_size << " filling to required size with ones" << std::endl;
            }
            if(input_string.size() > coefficients_size)
            {
                std::cerr << "WARN: Coefficients vector received for line number "<< res.size()+1 << " is to large: received a vector of size " << input_string.size() << " expected " << coefficients_size << " ignoring the extra values received" << std::endl;
            }
            
            for(unsigned i = 0; i < std::min(input_string.size() ,coefficients_size ); i++)
            {
                res_part.push_back( std::stod(input_string[i]));
            }
            
            while(res_part.size() < coefficients_size)
            {
                res_part.push_back(0);
            }
            
            res.push_back(res_part);
        }
    }
    
    while(res.size() < learning_fast_population)
    {
        //push ones to fill to required values
        std::vector<double> res_part;
        while(res_part.size() < coefficients_size)
        {
            res_part.push_back(0);
        }
        res.push_back(res_part);
    }
    
    learning_file.close();
    
    return res;
}

void Coefficient_db::write_learning_fast(std::vector<std::vector<double>> coefficient_value_table)
{
    std::fstream learning_file;
    learning_file.open("../coefficient_db/learning_fast.txt",std::ios::out);
    
    if(learning_file.is_open())
    {
        for(int row = 0;row<coefficient_value_table.size();row++)
        {
            for(int col = 0;col < coefficient_value_table[row].size();col++)
            {
                learning_file << (col>0? " ":"") << std::fixed << std::setprecision(15) << coefficient_value_table[row][col];
            }
            learning_file << std::endl;
        }
    }
    else
    {
        std::cerr << "WARN: Could not open learning file for writing, skipping write" << std::endl; 
    }
    
    learning_file.close();
}


std::vector<std::string> Coefficient_db::string_split(const std::string& str) 
{
	std::vector<std::string> result;
	std::istringstream iss(str);
	for (std::string s; iss >> s; )
    {
		result.push_back(s);
    }
	return result;
}