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
    Cell();
    ~Cell();
};