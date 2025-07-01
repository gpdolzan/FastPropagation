#pragma once

#include <iostream>
#include <vector>
#include <string>

class Tile
{
private:
    
public:
    std::string id;
    std::string north;
    std::string south;
    std::string east;
    std::string west;

    void set_tile_constraints(std::string constraints, std::string id);
    void print_tile_constraints(void);
    size_t get_memory_usage() const;
    Tile(/* args */);
    ~Tile();
};