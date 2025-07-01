#pragma once

#include <vector>
#include "Tile.hpp"

class Cell
{
private:
    
public:
    int collapsed;
    std::vector<Tile> domain;

    void print_domain(void);
    void print_domain_size(void);
    size_t get_memory_usage() const;
    Cell();
    ~Cell();
};