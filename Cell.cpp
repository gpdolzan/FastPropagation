#include "Cell.hpp"

void Cell::print_domain(void)
{
    std::cout << "============ DOMAIN ===========" << std::endl;
    for(int i = 0; i < domain.size(); i++)
    {
        domain[i].print_tile_constraints();
        std::cout << std::endl;
    }
    std::cout << "===============================" << std::endl;
}

void Cell::print_domain_size(void)
{
    std::cout << domain.size() << " ";
}

Cell::Cell()
{
    collapsed = -1;
}

Cell::~Cell()
{
}