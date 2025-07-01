#include "FastPropagation.hpp"
#include <iostream>
#include <algorithm>

void FastPropagation::initialize_fp(int rows, int columns, Cell c, unsigned int seed)
{
    this->rows = rows;
    this->columns = columns;
    rng.seed(seed);
    matrix.initialize_matrix(rows, columns, c);
    backtrack_count = 0; // Initialize counter
    backtrack_memory_cost = 0; // Initialize memory cost
}

void FastPropagation::run(std::string heuristic)
{
    if (heuristic == "FP")
    {
        FP(false);
    }
    else if (heuristic == "Diagonal")
    {
        Diag();
    }
}

void FastPropagation::FP(bool backtrack)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            bool success = false;
            while (!success) {
                if (backtrack) {
                    // Save state before attempting collapse
                    save_state(i, j, -1);
                    
                    // Try to collapse with backtracking support
                    success = collapse_with_backtrack(i, j);
                    if (!success) {
                        // If we couldn't collapse, try to backtrack
                        if (!backtrack_restore()) {
                            // If we can't backtrack further, the problem is unsolvable
                            std::cerr << "Error: Unable to solve - backtracking failed at (" << i << "," << j << ")" << std::endl;
                            return;
                        }
                        continue; // Try again from restored state
                    }
                } else {
                    // Original implementation without backtracking
                    collapse(i, j);
                    success = true;
                }
                
                // Propagate constraints
                propagate(i, j);
                
                if (backtrack && has_empty_domains()) {
                    // If propagation caused empty domains, we need to backtrack
                    success = false;
                    if (!backtrack_restore()) {
                        std::cerr << "Error: Unable to solve - propagation caused unsolvable state at (" << i << "," << j << ")" << std::endl;
                        return;
                    }
                } else {
                    success = true;
                }
            }
        }
    }
}

void FastPropagation::Diag()
{
    // Anti-diagonais: (0,0) -> (0,1), (1,0) -> (0,2), (1,1), (2,0) -> etc.
    for (int diagonal = 0; diagonal < rows + columns - 1; ++diagonal)
    {
        int start_row = std::max(0, diagonal - columns + 1);
        int end_row = std::min(rows - 1, diagonal);
        
        // Percorrer a anti-diagonal
        for (int row = start_row; row <= end_row; ++row)
        {
            int col = diagonal - row;
            
            if (col >= 0 && col < columns)
            {
                collapse(row, col);
                propagate(row, col);
            }
        }
    }
}

void FastPropagation::Diag(bool backtrack)
{
    // Anti-diagonais: (0,0) -> (0,1), (1,0) -> (0,2), (1,1), (2,0) -> etc.
    for (int diagonal = 0; diagonal < rows + columns - 1; ++diagonal)
    {
        int start_row = std::max(0, diagonal - columns + 1);
        int end_row = std::min(rows - 1, diagonal);
        
        // Percorrer a anti-diagonal
        for (int row = start_row; row <= end_row; ++row)
        {
            int col = diagonal - row;
            
            if (col >= 0 && col < columns)
            {
                bool success = false;
                while (!success) {
                    if (backtrack) {
                        // Save state before attempting collapse
                        save_state(row, col, -1);
                        
                        // Try to collapse with backtracking support
                        success = collapse_with_backtrack(row, col);
                        if (!success) {
                            // If we couldn't collapse, try to backtrack
                            if (!backtrack_restore()) {
                                // If we can't backtrack further, the problem is unsolvable
                                std::cerr << "Error: Unable to solve - backtracking failed at (" << row << "," << col << ")" << std::endl;
                                return;
                            }
                            continue; // Try again from restored state
                        }
                    } else {
                        // Original implementation without backtracking
                        collapse(row, col);
                        success = true;
                    }
                    
                    // Propagate constraints
                    propagate(row, col);
                    
                    if (backtrack && has_empty_domains()) {
                        // If propagation caused empty domains, we need to backtrack
                        success = false;
                        if (!backtrack_restore()) {
                            std::cerr << "Error: Unable to solve - propagation caused unsolvable state at (" << row << "," << col << ")" << std::endl;
                            return;
                        }
                    } else {
                        success = true;
                    }
                }
            }
        }
    }
}

void FastPropagation::collapse(int i, int j)
{
    int size = matrix.matrix[i][j].domain.size();
    std::uniform_int_distribution<std::size_t> dist(0, size - 1);
    int choice = dist(rng);

    Tile pickedValue = matrix.matrix[i][j].domain[choice];
    matrix.matrix[i][j].domain.clear();
    matrix.matrix[i][j].domain.push_back(pickedValue);
    matrix.matrix[i][j].collapsed = std::stoi(pickedValue.id);
}

void FastPropagation::propagate(int i, int j)
{
    Tile selected = matrix.matrix[i][j].domain[0]; // Tile that is collapsed

    if(i + 1 < rows) // Can remove NORTH
    {
        std::vector<Tile> remaining;
        for (auto & tile : matrix.matrix[i+1][j].domain)
        {
            if (selected.south == tile.north)
            {
                remaining.push_back(tile);
            }
        }
        matrix.matrix[i + 1][j].domain = std::move(remaining);
    }

    if (j + 1 < columns)  // Can remove WEST
    {
        std::vector<Tile> remaining;
        for (auto &tile : matrix.matrix[i][j+1].domain)
        {
            if (selected.east == tile.west)
            {
                remaining.push_back(tile);
            }
        }
        matrix.matrix[i][j+1].domain = std::move(remaining);
    }
}

FastPropagation::FastPropagation(/* args */)
{
    backtrack_count = 0;
    backtrack_memory_cost = 0;
}

FastPropagation::~FastPropagation()
{
}

bool FastPropagation::collapse_with_backtrack(int i, int j, const std::vector<int>& tried_tiles)
{
    std::vector<Tile> available_tiles;
    
    // Filter out tiles we've already tried
    for (const auto& tile : matrix.matrix[i][j].domain) {
        int tile_id = std::stoi(tile.id);
        if (std::find(tried_tiles.begin(), tried_tiles.end(), tile_id) == tried_tiles.end()) {
            available_tiles.push_back(tile);
        }
    }
    
    if (available_tiles.empty()) {
        return false; // No more tiles to try
    }
    
    // Select a random tile from available ones
    std::uniform_int_distribution<std::size_t> dist(0, available_tiles.size() - 1);
    int choice = dist(rng);
    
    Tile pickedValue = available_tiles[choice];
    matrix.matrix[i][j].domain.clear();
    matrix.matrix[i][j].domain.push_back(pickedValue);
    matrix.matrix[i][j].collapsed = std::stoi(pickedValue.id);
    
    // Update the state with the tile we just tried
    if (!state_stack.empty()) {
        state_stack.top().collapsed_tile_id = std::stoi(pickedValue.id);
        state_stack.top().tried_tiles.push_back(std::stoi(pickedValue.id));
    }
    
    return true;
}

bool FastPropagation::has_empty_domains()
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            if (matrix.matrix[i][j].collapsed == -1 && matrix.matrix[i][j].domain.empty()) {
                return true;
            }
        }
    }
    return false;
}

void FastPropagation::save_state(int i, int j, int collapsed_tile_id)
{
    BacktrackState state;
    state.matrix_state = matrix; // Use copy constructor
    state.row = i;
    state.col = j;
    state.collapsed_tile_id = collapsed_tile_id;
    state.tried_tiles.clear();
    
    // Calculate memory cost of this state save
    size_t state_memory = sizeof(BacktrackState) + state.matrix_state.get_memory_usage() + 
                         (state.tried_tiles.capacity() * sizeof(int));
    backtrack_memory_cost += state_memory;
    
    state_stack.push(state);
}

bool FastPropagation::backtrack_restore()
{
    if (state_stack.empty()) {
        return false; // Nothing to backtrack to
    }
    
    backtrack_count++; // Increment backtrack counter
    
    BacktrackState current_state = state_stack.top();
    state_stack.pop();
    
    // Check if we have more tiles to try at this position
    std::vector<Tile> available_tiles;
    for (const auto& tile : current_state.matrix_state.matrix[current_state.row][current_state.col].domain) {
        int tile_id = std::stoi(tile.id);
        if (std::find(current_state.tried_tiles.begin(), current_state.tried_tiles.end(), tile_id) == current_state.tried_tiles.end()) {
            available_tiles.push_back(tile);
        }
    }
    
    if (!available_tiles.empty()) {
        // We have more tiles to try, restore state and update tried_tiles
        matrix = current_state.matrix_state;
        
        // Put the state back with updated tried_tiles for next attempt
        state_stack.push(current_state);
        
        // Try to collapse with the updated tried_tiles list
        return collapse_with_backtrack(current_state.row, current_state.col, current_state.tried_tiles);
    } else {
        // No more tiles to try at this position, restore state and continue backtracking
        matrix = current_state.matrix_state;
        return backtrack_restore(); // Recursive backtrack
    }
}

int FastPropagation::get_backtrack_count() const
{
    return backtrack_count;
}

void FastPropagation::reset_backtrack_count()
{
    backtrack_count = 0;
    backtrack_memory_cost = 0;
}

size_t FastPropagation::get_backtrack_memory_cost() const
{
    return backtrack_memory_cost;
}

size_t FastPropagation::get_total_backtrack_impact() const
{
    // Combine backtrack count (operations) + memory cost (bytes)
    // Weight the count to make it comparable to memory (each backtrack = 1KB equivalent impact)
    return (backtrack_count * 1024) + backtrack_memory_cost;
}

size_t FastPropagation::get_matrix_memory_usage() const
{
    return matrix.get_memory_usage();
}

size_t FastPropagation::get_backtrack_stack_memory_usage() const
{
    if (state_stack.empty()) {
        return sizeof(state_stack); // Just the empty stack container
    }
    
    size_t size = sizeof(state_stack); // Stack container itself
    
    // Each BacktrackState contains a matrix copy, so we estimate the memory per state
    // This is an approximation since we can't iterate through std::stack
    size_t matrix_memory = matrix.get_memory_usage();
    size_t per_state_size = sizeof(BacktrackState) + matrix_memory;
    
    size += state_stack.size() * per_state_size;
    
    return size;
}

size_t FastPropagation::get_memory_usage() const
{
    size_t size = 0;
    
    // Base object size
    size += sizeof(*this);
    
    // Matrix memory
    size += get_matrix_memory_usage() - sizeof(Matrix); // Subtract base Matrix size to avoid double counting
    
    // Backtrack stack memory
    size += get_backtrack_stack_memory_usage() - sizeof(state_stack); // Subtract base stack size
    
    // Random number generator (minimal)
    size += sizeof(rng);
    
    return size;
}