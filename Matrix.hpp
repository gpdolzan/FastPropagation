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
    size_t get_memory_usage() const;
    Matrix();
    Matrix(const Matrix& other); // Copy constructor
    Matrix& operator=(const Matrix& other); // Assignment operator
    ~Matrix();
};