#include "WFC.hpp"
#include <climits>

void WFC::initialize_wfc(int rows, int columns, Cell c, unsigned int seed)
{
    this->rows = rows;
    this->columns = columns;
    rng.seed(seed);
    matrix.initialize_matrix(rows, columns, c);
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

                // Backtracking aqui se domain_size == 0

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
            break;

        collapse(r, c);
        propagate(r, c); // AC-3 aqui
    }
}

void WFC::Diag()
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