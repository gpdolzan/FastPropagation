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
    rows = 0;
    columns = 0;
}

Matrix::Matrix(const Matrix& other)
{
    rows = other.rows;
    columns = other.columns;
    matrix = other.matrix;
}

Matrix& Matrix::operator=(const Matrix& other)
{
    if (this != &other) {
        rows = other.rows;
        columns = other.columns;
        matrix = other.matrix;
    }
    return *this;
}

Matrix::~Matrix()
{
}

size_t Matrix::get_memory_usage() const
{
    size_t size = 0;
    size += sizeof(*this); // Base object size
    
    // Calculate memory for the 2D vector structure
    size += sizeof(std::vector<Cell>) * matrix.capacity(); // Outer vector capacity
    
    for (const auto& row : matrix) {
        size += sizeof(Cell) * row.capacity(); // Inner vector capacity
        for (const auto& cell : row) {
            size += cell.get_memory_usage() - sizeof(Cell); // Cell content minus base size
        }
    }
    
    return size;
}