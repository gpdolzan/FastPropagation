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
    rng.seed(seed);
    matrix.initialize_matrix(this->rows, this->columns, c);

    //std::cout << "NWFC Grid is: " << this->rows << "x" << this->columns << std::endl;
}

void NWFC::run()
{
    int subgrids_rows = (rows - 1) / (subgrid_size - 1);
    int subgrids_cols = (columns - 1) / (subgrid_size - 1);
    
    // Loop para percorrer as subgrids
    for (int subgrid_row = 0; subgrid_row < subgrids_rows; ++subgrid_row)
    {
        for (int subgrid_col = 0; subgrid_col < subgrids_cols; ++subgrid_col)
        {
            int start_row = subgrid_row * (subgrid_size - 1);
            int start_col = subgrid_col * (subgrid_size - 1);
            
            WFC subgrid_wfc;
            Cell base_cell;
            base_cell.domain = original_domain;
            subgrid_wfc.initialize_wfc(subgrid_size, subgrid_size, base_cell, rng());
            
            // Copia para matriz adicional
            for (int i = 0; i < subgrid_size; ++i)
            {
                for (int j = 0; j < subgrid_size; ++j)
                {
                    int main_i = start_row + i;
                    int main_j = start_col + j;
                    
                    subgrid_wfc.matrix.matrix[i][j] = matrix.matrix[main_i][main_j];
                }
            }
            
            if (subgrid_row > 0 || subgrid_col > 0) // AC-3 se não for o primeiro subgrid
            {
                for (int i = 0; i < subgrid_size; ++i)
                {
                    for (int j = 0; j < subgrid_size; ++j)
                    {
                        if (subgrid_wfc.matrix.matrix[i][j].collapsed != -1)
                        {
                            bool is_border_cell = false;
                            
                            // Borda de cima
                            if (subgrid_row > 0 && i == 0)
                                is_border_cell = true;
                            
                            // Borda esquerda
                            if (subgrid_col > 0 && j == 0)
                                is_border_cell = true;
                            
                            // Propaga se for de borda
                            if (is_border_cell)
                            {
                                subgrid_wfc.propagate(i, j);
                            }
                        }
                    }
                }
            }
            
            subgrid_wfc.MRV();
            
            // Copia de volta
            for (int i = 0; i < subgrid_size; ++i)
            {
                for (int j = 0; j < subgrid_size; ++j)
                {
                    int main_i = start_row + i;
                    int main_j = start_col + j;
                    
                    matrix.matrix[main_i][main_j] = subgrid_wfc.matrix.matrix[i][j];
                }
            }
        }
    }
}

NWFC::NWFC(/* args */)
{
}

NWFC::~NWFC()
{
}