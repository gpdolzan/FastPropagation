#pragma once

#include <random>
#include <vector>
#include "Matrix.hpp"
#include "Tile.hpp"

class NWFC
{
private:
    
public:
    int rows;
    int columns;
    int subgrid_size;
    std::vector<Tile> original_domain;
    Matrix matrix;
    std::mt19937 rng;

    void initialize_nwfc(int rows, int columns, int subgrid_size, Cell c, unsigned int seed);
    void run();
    NWFC(/* args */);
    ~NWFC();
};