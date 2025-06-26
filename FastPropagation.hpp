#pragma once

#include <random>
#include "Matrix.hpp"

class FastPropagation
{
private:
    
public:
    int rows;
    int columns;
    Matrix matrix;
    std::mt19937 rng;

    void initialize_fp(int rows, int columns, Cell c, unsigned int seed);
    void run(void);
    void collapse(int i, int j);
    void propagate(int i, int j);
    FastPropagation(/* args */);
    ~FastPropagation();
};