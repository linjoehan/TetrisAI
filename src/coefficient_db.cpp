#include "coefficient_db.h"

Coefficient_db::Coefficient_db():
coefficients_size(4)
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
            std::cerr << "WARN: Coefficients vector received is to small: received a vector of size " << string_default.size() << " expected " << coefficients_size << " filling to required size with zeros" << std::endl;
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