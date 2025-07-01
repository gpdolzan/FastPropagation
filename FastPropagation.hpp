#pragma once

#include <random>
#include <stack>
#include "Matrix.hpp"

struct BacktrackState {
    Matrix matrix_state;
    int row;
    int col;
    int collapsed_tile_id;
    std::vector<int> tried_tiles; // Keep track of tiles we've already tried
};

class FastPropagation
{
private:
    std::stack<BacktrackState> state_stack;
    int backtrack_count;
    size_t backtrack_memory_cost; // Total memory cost of all backtrack operations
    
public:
    int rows;
    int columns;
    Matrix matrix;
    std::mt19937 rng;

    void initialize_fp(int rows, int columns, Cell c, unsigned int seed);
    void run(std::string heuristic);
    void FP(bool backtrack);
    void Diag();
    void Diag(bool backtrack);
    void collapse(int i, int j);
    bool collapse_with_backtrack(int i, int j, const std::vector<int>& tried_tiles = {});
    void propagate(int i, int j);
    bool has_empty_domains();
    void save_state(int i, int j, int collapsed_tile_id);
    bool backtrack_restore();
    int get_backtrack_count() const;
    void reset_backtrack_count();
    size_t get_backtrack_memory_cost() const;
    size_t get_total_backtrack_impact() const; // Combines count + memory cost
    size_t get_memory_usage() const;
    size_t get_matrix_memory_usage() const;
    size_t get_backtrack_stack_memory_usage() const;
    FastPropagation(/* args */);
    ~FastPropagation();
};