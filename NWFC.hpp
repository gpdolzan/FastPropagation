#pragma once

#include <random>
#include <vector>
#include "Matrix.hpp"
#include "Tile.hpp"

class NWFC
{
private:
    int total_backtracks;
    size_t total_backtrack_memory;
    
public:
    int rows;
    int columns;
    int subgrid_size;
    std::vector<Tile> original_domain;
    Matrix matrix;
    std::mt19937 rng;

    void initialize_nwfc(int rows, int columns, int subgrid_size, Cell c, unsigned int seed);
    void run(bool enable_backtracking = false);
    size_t get_memory_usage() const;
    size_t get_matrix_memory_usage() const;
    int get_total_backtrack_count() const;
    size_t get_total_backtrack_stack_memory_usage() const;
    NWFC(/* args */);
    ~NWFC();
};