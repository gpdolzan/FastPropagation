#include "WFC.hpp"
#include <climits>
#include <algorithm>
#include <iostream>

void WFC::initialize_wfc(int rows, int columns, Cell c, unsigned int seed)
{
    this->rows = rows;
    this->columns = columns;
    rng.seed(seed);
    matrix.initialize_matrix(rows, columns, c);
    
    // Initialize backtracking variables
    backtrack_count = 0;
    backtrack_memory_cost = 0;
    while (!state_stack.empty()) {
        state_stack.pop();
    }
}

void WFC::run(std::string heuristic)
{
    if(heuristic == "MRV")
        MRV();
    else if (heuristic == "Diagonal")
        Diag();
}

void WFC::MRV()
{
    MRV(false); // Default to no backtracking
}

void WFC::MRV(bool backtrack)
{
    while (true)
    {
        int smallest_domain = INT_MAX;
        int r = -1;
        int c = -1;

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                // Skip already collapsed cells
                if (matrix.matrix[i][j].collapsed != -1)
                {
                    continue;
                }

                int domain_size = matrix.matrix[i][j].domain.size(); // Entropy

                // Check for empty domain
                if (domain_size == 0)
                {
                    if (backtrack && !state_stack.empty())
                    {
                        if (!backtrack_restore())
                        {
                            return; // Failed to find solution
                        }
                        continue; // Restart the loop after backtracking
                    }
                    else
                    {
                        return; // No backtracking enabled or no states to restore, exit
                    }
                }

                // Find cell with minimum entropy
                if (smallest_domain > domain_size)
                {
                    smallest_domain = domain_size;
                    r = i;
                    c = j;
                }
            }
        }

        if (r == -1)
            break; // All cells are collapsed

        // Collapse with or without backtracking
        if (backtrack)
        {
            bool success = false;
            while (!success) {
                // Save state before attempting any collapse on this cell
                save_state(r, c, -1);
                
                // Try to collapse with backtracking support
                success = collapse_with_backtrack(r, c, {});
                
                if (!success) {
                    // Remove the failed state and try to backtrack
                    state_stack.pop();
                    
                    if (!state_stack.empty()) {
                        if (!backtrack_restore()) {
                            std::cerr << "Error: Unable to solve - backtracking failed at (" << r << "," << c << ")" << std::endl;
                            return;
                        }
                        continue; // Try again from restored state
                    } else {
                        std::cerr << "Error: Unable to solve - no states to restore at (" << r << "," << c << ")" << std::endl;
                        return;
                    }
                }
                
                // Propagate constraints
                propagate(r, c);
                
                // Check for contradictions after propagation
                if (has_empty_domains()) {
                    // If propagation caused empty domains, we need to backtrack
                    success = false;
                    if (!state_stack.empty()) {
                        if (!backtrack_restore()) {
                            std::cerr << "Error: Unable to solve - propagation caused unsolvable state at (" << r << "," << c << ")" << std::endl;
                            return;
                        }
                    } else {
                        std::cerr << "Error: Unable to solve - propagation failed with no backtrack states at (" << r << "," << c << ")" << std::endl;
                        return;
                    }
                } else {
                    success = true;
                }
            }
        }
        else
        {
            collapse(r, c);
            propagate(r, c);
        }
    }
}

void WFC::Diag()
{
    Diag(false); // Default to no backtracking
}

void WFC::Diag(bool backtrack)
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
                // Check if cell is already collapsed
                if (matrix.matrix[row][col].collapsed != -1)
                    continue;
                
                bool success = false;
                while (!success) {
                    if (backtrack) {
                        // Save state before attempting collapse
                        save_state(row, col, -1);
                        
                        // Try to collapse with backtracking support
                        success = collapse_with_backtrack(row, col, {});
                        if (!success) {
                            // Remove the failed state and try to backtrack
                            state_stack.pop();
                            
                            if (!state_stack.empty()) {
                                if (!backtrack_restore()) {
                                    std::cerr << "Error: Unable to solve - backtracking failed at (" << row << "," << col << ")" << std::endl;
                                    return;
                                }
                                continue; // Try again from restored state
                            } else {
                                std::cerr << "Error: Unable to solve - no states to restore at (" << row << "," << col << ")" << std::endl;
                                return;
                            }
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
                        if (!state_stack.empty()) {
                            if (!backtrack_restore()) {
                                std::cerr << "Error: Unable to solve - propagation caused unsolvable state at (" << row << "," << col << ")" << std::endl;
                                return;
                            }
                        } else {
                            std::cerr << "Error: Unable to solve - propagation failed with no backtrack states at (" << row << "," << col << ")" << std::endl;
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

void WFC::collapse(int i, int j)
{
    int size = matrix.matrix[i][j].domain.size();
    std::uniform_int_distribution<std::size_t> dist(0, size - 1);
    int choice = dist(rng);

    Tile pickedValue = matrix.matrix[i][j].domain[choice];
    matrix.matrix[i][j].domain.clear();
    matrix.matrix[i][j].domain.push_back(pickedValue);
    matrix.matrix[i][j].collapsed = std::stoi(pickedValue.id);

    //std::cout << "Colapsando " << i << " " << j << std::endl;
}


// Exemplo de teste de compatibilidade; adapte aos seus campos:
bool is_compatible(const Tile& t_from, const Tile& t_to, int direction) {
    // se direction == 0 (Norte), testamos t_from.north vs t_to.south, etc.
    switch(direction) {
        case 0: return t_from.north == t_to.south;
        case 1: return t_from.east  == t_to.west;
        case 2: return t_from.south == t_to.north;
        case 3: return t_from.west  == t_to.east;
    }
    return false;
}

void WFC::propagate(int start_i, int start_j)
{
    const int dRow[4] = { -1,  0, +1,  0 };
    const int dColumn[4] = {  0, +1,  0, -1 };

    std::deque<std::tuple<int,int,int>> queue; // (linha, coluna, direção de onde veio)
    // inicializa a fila com os 4 arcos vindos da célula colapsada
    for (int dir = 0; dir < 4; ++dir)
    {
        queue.emplace_back(start_i + dRow[dir], start_j + dColumn[dir], (dir + 2) % 4 ); // direção inversa, do ponto de vista do vizinho
    }

    while (!queue.empty())
    {
        auto [i, j, dir_from_neighbor] = queue.front();
        queue.pop_front();

        // se não existe ou se já foi colapsada, pula
        if (i<0 || i>=rows || j<0 || j>=columns) continue;
        if (matrix.matrix[i][j].collapsed != -1) continue;

        bool revised = false;
        auto& domain_ij = matrix.matrix[i][j].domain;

        // domínio temporário para reconstruir só com padrões suportados:
        std::vector<Tile> new_domain;
        new_domain.reserve(domain_ij.size());

        for (auto& tile_ij : domain_ij)
        {
            bool has_support = false;
            // olha o vizinho naquela direção:
            int ni = i + dRow[dir_from_neighbor];
            int nj = j + dColumn[dir_from_neighbor];
            if (ni<0 || ni>=rows || nj<0 || nj>=columns)
            {
                // sem vizinho, assume que sempre suporta
                has_support = true;
            }
            else
            {
                const auto& domain_n = matrix.matrix[ni][nj].domain;
                // se ao menos um padrão no vizinho for compatível:
                for (auto& tile_n : domain_n)
                {
                    if (is_compatible(tile_ij, tile_n, dir_from_neighbor))
                    {
                        has_support = true;
                        break;
                    }
                }
            }
            if (has_support)
            {
                new_domain.push_back(tile_ij);
            }
            else
            {
                revised = true;
            }
        }

        if (revised)
        {
            // domínio mudou: aplica a nova lista
            domain_ij.swap(new_domain);

            // reenfileira todos os arcos vindos dos demais vizinhos
            for (int dir2 = 0; dir2 < 4; ++dir2)
            {
                int pi = i + dRow[dir2];
                int pj = j + dColumn[dir2];
                // não reenfileirar o arco de volta para onde já veio agora:
                if (pi<0 || pi>=rows || pj<0 || pj>=columns) continue;
                if (pi == start_i && pj == start_j) continue;
                queue.emplace_back(pi, pj, (dir2+2)%4);
            }
        }
    } 

    //std::cout << "Propagation Ended!" << std::endl;
}

WFC::WFC(/* args */)
{
}

WFC::~WFC()
{
}

void WFC::reset_matrix(Cell c)
{
    // Reset the matrix to initial state
    matrix.initialize_matrix(rows, columns, c);
}

size_t WFC::get_matrix_memory_usage() const
{
    return matrix.get_memory_usage();
}

size_t WFC::get_memory_usage() const
{
    size_t size = 0;
    
    // Base object size
    size += sizeof(*this);
    
    // Matrix memory
    size += get_matrix_memory_usage() - sizeof(Matrix); // Subtract base Matrix size to avoid double counting
    
    // Random number generator (minimal)
    size += sizeof(rng);
    
    // Backtracking stack memory (current usage)
    size += get_backtrack_stack_memory_usage();
    
    return size;
}

bool WFC::collapse_with_backtrack(int i, int j, const std::vector<int>& tried_tiles)
{
    // Get available tiles that haven't been tried yet
    std::vector<Tile> available_tiles;
    for (const auto& tile : matrix.matrix[i][j].domain) {
        int tile_id = std::stoi(tile.id);
        if (std::find(tried_tiles.begin(), tried_tiles.end(), tile_id) == tried_tiles.end()) {
            available_tiles.push_back(tile);
        }
    }
    
    if (available_tiles.empty()) {
        return false; // No more tiles to try
    }
    
    // Randomly select a tile from available ones
    std::uniform_int_distribution<std::size_t> dist(0, available_tiles.size() - 1);
    int choice = dist(rng);
    Tile pickedValue = available_tiles[choice];
    int collapsed_tile_id = std::stoi(pickedValue.id);
    
    // Perform the collapse
    matrix.matrix[i][j].domain.clear();
    matrix.matrix[i][j].domain.push_back(pickedValue);
    matrix.matrix[i][j].collapsed = collapsed_tile_id;
    
    // Update the state with the tile we just tried
    if (!state_stack.empty()) {
        state_stack.top().collapsed_tile_id = collapsed_tile_id;
        state_stack.top().tried_tiles.push_back(collapsed_tile_id);
    }
    
    return true;
}

bool WFC::has_empty_domains()
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

void WFC::save_state(int i, int j, int collapsed_tile_id)
{
    WFCBacktrackState state;
    state.matrix_state = matrix; // Save current matrix state
    state.row = i;
    state.col = j;
    state.collapsed_tile_id = -1; // Will be set when we actually try tiles
    state.tried_tiles.clear(); // Start with empty list
    
    // Calculate memory cost
    size_t state_memory = sizeof(WFCBacktrackState) + state.matrix_state.get_memory_usage();
    backtrack_memory_cost += state_memory;
    
    state_stack.push(state);
}

bool WFC::backtrack_restore()
{
    if (state_stack.empty()) {
        return false; // Nothing to backtrack to
    }
    
    backtrack_count++; // Increment backtrack counter
    
    WFCBacktrackState current_state = state_stack.top();
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

int WFC::get_backtrack_count() const
{
    return backtrack_count;
}

void WFC::reset_backtrack_count()
{
    backtrack_count = 0;
    backtrack_memory_cost = 0;
    while (!state_stack.empty()) {
        state_stack.pop();
    }
}

size_t WFC::get_backtrack_memory_cost() const
{
    return backtrack_memory_cost;
}

size_t WFC::get_total_backtrack_impact() const
{
    return backtrack_memory_cost + (backtrack_count * sizeof(WFCBacktrackState));
}

size_t WFC::get_backtrack_stack_memory_usage() const
{
    size_t total = 0;
    // Calculate current stack size
    std::stack<WFCBacktrackState> temp_stack = state_stack; // Copy the stack
    while (!temp_stack.empty()) {
        total += sizeof(WFCBacktrackState) + temp_stack.top().matrix_state.get_memory_usage();
        temp_stack.pop();
    }
    return total;
}