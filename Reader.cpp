#include "Reader.hpp"

void Reader::read_files(std::string filepath)
{
    for (const auto & entry : std::filesystem::directory_iterator(filepath))
    {
        // Grab all the filenames without path or extensions (Ex: "CCCC")
        std::string constraints_str = entry.path().stem().generic_string();

        constraints.push_back(constraints_str);
    }
}

void Reader::print_constraints(void)
{
    std::cout << "Number of constraints: " << Reader::constraints.size() << std::endl;
    for (const auto & it : Reader::constraints)
    {
        std::cout << it << std::endl;
    }
}

std::vector<Tile> Reader::generate_domain(void)
{
    std::vector<Tile> domain;

    // Initialize domain with every possible tile
    for (int i = 0; i < constraints.size(); i++)
    {
        Tile t;
        t.set_tile_constraints(constraints[i], std::to_string(i));
        domain.push_back(t);
    }

    return domain;
}

Reader::Reader(/* args */)
{
}

Reader::~Reader()
{
}
