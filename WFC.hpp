#pragma once

#include <random>
#include <tuple>
#include <deque>
#include "Matrix.hpp"

class WFC
{
private:
    
public:
    int rows;
    int columns;
    Matrix matrix;
    std::mt19937 rng;

    void initialize_wfc(int rows, int columns, Cell c, unsigned int seed);
    void run(std::string heuristic);
    void Diag();
    void MRV();
    void collapse(int i, int j);
    void propagate(int i, int j);
    WFC(/* args */);
    ~WFC();
};