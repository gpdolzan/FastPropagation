#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include "Tile.hpp"

class Reader
{
private:
    
public:
    std::vector<std::string> constraints;

    void read_files(std::string filepath);
    void print_constraints(void);
    std::vector<Tile> generate_domain(void);
    Reader(/* args */);
    ~Reader();
};