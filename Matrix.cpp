#include "Matrix.hpp"

void Matrix::initialize_matrix(int rows, int columns, Cell c)
{
    this->rows = rows;
    this->columns = columns;
    matrix.assign(rows, std::vector<Cell>(columns, c));
    //std::vector<std::vector<Cell>> mat(rows, std::vector<Cell>(columns, c));
    //matrix = mat;
}

void Matrix::print_possibilities(void)
{
    std::cout << "======== POSSIBILITIES ========" << std::endl;
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < columns; j++)
        {
            matrix[i][j].print_domain_size();
        }
        std::cout << std::endl;
    }
    std::cout << "===============================" << std::endl;
}

void Matrix::print_ids(void)
{
    std::cout << "============== ID =============" << std::endl;
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < columns; j++)
        {
            std::cout << std::setw(2) << std::setfill('0') << matrix[i][j].collapsed << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "===============================" << std::endl;
}

Matrix::Matrix()
{
    
}

Matrix::~Matrix()
{
}