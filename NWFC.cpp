#include "NWFC.hpp"
#include "WFC.hpp"
#include <algorithm>
#include <iostream>

void NWFC::initialize_nwfc(int rows, int columns, int subgrid_size, Cell c, unsigned int seed)
{
    this->subgrid_size = subgrid_size;
    this->original_domain = c.domain; // Store the original domain
    this->rows = (rows * (subgrid_size - 1) + 1);
    this->columns = (columns * (subgrid_size - 1) + 1);
    this->total_backtracks = 0;
    this->total_backtrack_memory = 0;
    rng.seed(seed);
    matrix.initialize_matrix(this->rows, this->columns, c);

    //std::cout << "NWFC Grid is: " << this->rows << "x" << this->columns << std::endl;
}

void NWFC::run(bool enable_backtracking)
{
    int subgrids_rows = (rows - 1) / (subgrid_size - 1);
    int subgrids_cols = (columns - 1) / (subgrid_size - 1);

    // Reset stats
    total_backtracks = total_backtrack_memory = 0;

    for (int subgrid_row = 0; subgrid_row < subgrids_rows; ++subgrid_row) {
        for (int subgrid_col = 0; subgrid_col < subgrids_cols; ++subgrid_col) {
            int start_row = subgrid_row * (subgrid_size - 1);
            int start_col = subgrid_col * (subgrid_size - 1);

            // Determine if we need a phantom border
            bool add_bottom = enable_backtracking && (subgrid_row < subgrids_rows - 1);
            bool add_right  = enable_backtracking && (subgrid_col < subgrids_cols - 1);

            int wfc_rows = subgrid_size + (add_bottom ? 1 : 0);
            int wfc_cols = subgrid_size + (add_right  ? 1 : 0);

            // Initialize the WFC solver on the extended window
            WFC subgrid_wfc;
            Cell base_cell; base_cell.domain = original_domain;
            subgrid_wfc.initialize_wfc(wfc_rows, wfc_cols, base_cell, rng());

            // Copy the current global state into the top-left of subgrid_wfc
            for (int i = 0; i < subgrid_size; ++i) {
                for (int j = 0; j < subgrid_size; ++j) {
                    int gi = start_row + i;
                    int gj = start_col + j;
                    subgrid_wfc.matrix.matrix[i][j] = matrix.matrix[gi][gj];
                }
            }
            // (The extra bottom/right row/col remain at base_cell.domain)

            // If not the very first subgrid, do AC-3 propagation on its top/left border
            if (subgrid_row > 0 || subgrid_col > 0) {
                for (int i = 0; i < wfc_rows; ++i) {
                    for (int j = 0; j < wfc_cols; ++j) {
                        if (subgrid_wfc.matrix.matrix[i][j].collapsed != -1) {
                            bool is_border =
                                (subgrid_row > 0 && i == 0) ||     // top edge
                                (subgrid_col > 0 && j == 0);      // left edge
                            if (is_border) subgrid_wfc.propagate(i, j);
                        }
                    }
                }
            }

            // Collapse!
            if (enable_backtracking) {
                subgrid_wfc.MRV(true);
                total_backtracks      += subgrid_wfc.get_backtrack_count();
                total_backtrack_memory += subgrid_wfc.get_backtrack_stack_memory_usage();
            } else {
                subgrid_wfc.MRV();
            }

            // Copy **only** the original subgrid back into the global matrix
            for (int i = 0; i < subgrid_size; ++i) {
                for (int j = 0; j < subgrid_size; ++j) {
                    int gi = start_row + i;
                    int gj = start_col + j;
                    matrix.matrix[gi][gj] = subgrid_wfc.matrix.matrix[i][j];
                }
            }
        }
    }
}

size_t NWFC::get_matrix_memory_usage() const
{
    return matrix.get_memory_usage();
}

size_t NWFC::get_memory_usage() const
{
    size_t size = 0;
    
    // Base object size
    size += sizeof(*this);
    
    // Matrix memory
    size += get_matrix_memory_usage() - sizeof(Matrix); // Subtract base Matrix size to avoid double counting
    
    // Random number generator (minimal)
    size += sizeof(rng);
    
    // Original domain vector
    size += original_domain.capacity() * sizeof(Tile);
    for (const auto& tile : original_domain) {
        size += tile.get_memory_usage() - sizeof(Tile); // Subtract base Tile size to avoid double counting
    }
    
    return size;
}

int NWFC::get_total_backtrack_count() const
{
    return total_backtracks;
}

size_t NWFC::get_total_backtrack_stack_memory_usage() const
{
    return total_backtrack_memory;
}

NWFC::NWFC(/* args */)
{
}

NWFC::~NWFC()
{
}