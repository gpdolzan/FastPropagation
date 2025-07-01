#pragma once

#include <random>
#include <tuple>
#include <deque>
#include <stack>
#include "Matrix.hpp"

struct WFCBacktrackState {
    Matrix matrix_state;
    int row;
    int col;
    int collapsed_tile_id;
    std::vector<int> tried_tiles; // Keep track of tiles we've already tried
};

class WFC
{
private:
    std::stack<WFCBacktrackState> state_stack;
    int backtrack_count;
    size_t backtrack_memory_cost; // Total memory cost of all backtrack operations
    
public:
    int rows;
    int columns;
    Matrix matrix;
    std::mt19937 rng;

    void initialize_wfc(int rows, int columns, Cell c, unsigned int seed);
    void run(std::string heuristic);
    void Diag();
    void Diag(bool backtrack);
    void MRV();
    void MRV(bool backtrack);
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
    size_t get_backtrack_stack_memory_usage() const;
    void reset_matrix(Cell c); // New method to reset the matrix
    size_t get_memory_usage() const;
    size_t get_matrix_memory_usage() const;
    WFC(/* args */);
    ~WFC();
};