#ifndef COEFFICIENT_DB_H
#define COEFFICIENT_DB_H

#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class Coefficient_db
{
    public:
    Coefficient_db();
    const unsigned coefficients_size;
    const unsigned learning_population;
    
    std::vector<double> get_default();
    void write_default(std::vector<double> coefficient_value);
    std::vector<std::vector<double>> get_learning();
    void write_learning(std::vector<std::vector<double>> coefficient_value_table);
    
    private:
    std::vector<std::string> string_split(const std::string& str);
};

#endif