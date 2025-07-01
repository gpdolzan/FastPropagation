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

size_t Cell::get_memory_usage() const
{
    size_t size = 0;
    size += sizeof(*this); // Base object size
    size += sizeof(Tile) * domain.capacity(); // Vector capacity for Tile objects
    
    // Add memory for each tile's strings
    for (const auto& tile : domain) {
        size += tile.get_memory_usage() - sizeof(Tile); // Subtract base size to avoid double counting
    }
    
    return size;
}