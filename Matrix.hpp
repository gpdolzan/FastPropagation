#pragma once

#include <iomanip>
#include "Cell.hpp"

class Matrix
{
private:
    
public:
    std::vector<std::vector<Cell>> matrix;
    int rows;
    int columns;

    void initialize_matrix(int rows, int columns, Cell c);
    void print_possibilities(void);
    void print_ids(void);
    Matrix();
    ~Matrix();
};