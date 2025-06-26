#include "FastPropagation.hpp"

void FastPropagation::initialize_fp(int rows, int columns, Cell c, unsigned int seed)
{
    this->rows = rows;
    this->columns = columns;
    rng.seed(seed);
    matrix.initialize_matrix(rows, columns, c);
}

void FastPropagation::run(void)
{
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < columns; j++)
        {
            collapse(i, j);
            propagate(i, j);
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
}

FastPropagation::~FastPropagation()
{
}