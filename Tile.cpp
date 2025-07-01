#include "Tile.hpp"

void Tile::set_tile_constraints(std::string constraints, std::string id)
{
    this->id = id;
    north = constraints[0];
    south = constraints[1];
    east = constraints[2];
    west = constraints[3];
}

void Tile::print_tile_constraints(void)
{
    std::cout << "ID: " << id << std::endl;
    std::cout << "North: " << north << std::endl;
    std::cout << "South: " << south << std::endl;
    std::cout << "East: " << east << std::endl;
    std::cout << "West: " << west << std::endl;
}

size_t Tile::get_memory_usage() const
{
    size_t size = 0;
    size += sizeof(*this); // Base object size
    size += id.capacity(); // String memory
    size += north.capacity();
    size += south.capacity();
    size += east.capacity();
    size += west.capacity();
    return size;
}

Tile::Tile(/* args */)
{
}

Tile::~Tile()
{
}