#include "Reader.hpp"
#include "Tile.hpp"
#include "Cell.hpp"
#include "FastPropagation.hpp"
#include "ImageGenerator.hpp"
#include "WFC.hpp"
#include "NWFC.hpp"
#include <chrono>
#include <string>
#include <iostream>

void print_usage(const char* program_name) {
    std::cout << "Argumentos:\n";
    std::cout << "  algoritmo: FP, FP_DIAGONAL, WFC, WFC_DIAGONAL, NWFC\n";
    std::cout << "  pasta: Tilesets -> Roads, Raods--, Roads++, Carcassonne, Carcassonne++\n";
    std::cout << "  grid_size: Size of the grid (e.g., 10 for 10x10)\n";
    std::cout << "  seed: Random seed (integer)\n";
    std::cout << "  gerar_imagem: 1 gera iamgem, 0 nao gera\n";
    std::cout << "  subgrid_size: necessário se for usar o NWFC onde o tamanho_subgrid >= 2\n";
    std::cout << "Usage: main <algoritmo> <pasta> <tamanho_matriz> <seed> <gerar_imagem> [tamanho_subgrid]\n";
    std::cout << "Exemplos:\n";
    std::cout << "main WFC Roads 10 1234 1\n";
    std::cout << "main NWFC Assets 20 5678 0 3\n";
    std::cout << "main FP_DIAGONAL Roads++ 15 9999 1\n";
}

int main(int argc, char const *argv[])
{
    if (argc < 6) {
        print_usage(argv[0]);
        return 1;
    }

    // Parse command line arguments
    std::string algorithm = argv[1];
    std::string folder = argv[2];
    int grid_size = std::stoi(argv[3]);
    int seed = std::stoi(argv[4]);
    bool generate_image = (std::stoi(argv[5]) == 1);
    int subgrid_size = 2; // default
    
    if (algorithm == "NWFC") {
        if (argc < 7) {
            std::cout << "Error: NWFC requires subgrid_size parameter\n";
            print_usage(argv[0]);
            return 1;
        }
        subgrid_size = std::stoi(argv[6]);
    }

    // Fixed parameters
    std::string output_file = algorithm + "_" + folder + "_" + std::to_string(grid_size) + "_" + std::to_string(seed) + ".png";

    std::cout << "Running " << algorithm << " on " << grid_size << "x" << grid_size 
              << " grid with tileset '" << folder << "' and seed " << seed;
    if (algorithm == "NWFC") {
        std::cout << " (subgrid_size=" << subgrid_size << ")";
    }
    std::cout << std::endl;

    Reader r;
    Cell c;
    ImageGenerator ig;

    // Chrono
    using Clock = std::chrono::high_resolution_clock;
    using Milliseconds = std::chrono::duration<double, std::milli>;

    // Read constraints
    auto t_start = Clock::now();
    r.read_files(folder);
    c.domain = r.generate_domain();
    auto t_end = Clock::now();
    Milliseconds ms_read = t_end - t_start;

    // Initialize and run algorithm
    t_start = Clock::now();
    auto init_start = Clock::now();
    
    if (algorithm == "FP") {
        FastPropagation fp;
        fp.initialize_fp(grid_size, grid_size, c, seed);
        auto init_end = Clock::now();
        Milliseconds ms_init = init_end - init_start;
        
        auto run_start = Clock::now();
        fp.run("FP");
        auto run_end = Clock::now();
        Milliseconds ms_run = run_end - run_start;
        
        std::cout << "Init: " << ms_init.count() << " ms, Run: " << ms_run.count() << " ms\n";
        
        if (generate_image) {
            ig.initialize(r, folder);
            ig.generate_image(fp.matrix, output_file);
        }
    }
    else if (algorithm == "FP_DIAGONAL") {
        FastPropagation fp;
        fp.initialize_fp(grid_size, grid_size, c, seed);
        auto init_end = Clock::now();
        Milliseconds ms_init = init_end - init_start;
        
        auto run_start = Clock::now();
        fp.run("Diagonal");
        auto run_end = Clock::now();
        Milliseconds ms_run = run_end - run_start;
        
        std::cout << "Init: " << ms_init.count() << " ms, Run: " << ms_run.count() << " ms\n";
        
        if (generate_image) {
            ig.initialize(r, folder);
            ig.generate_image(fp.matrix, output_file);
        }
    }
    else if (algorithm == "WFC") {
        WFC wfc;
        wfc.initialize_wfc(grid_size, grid_size, c, seed);
        auto init_end = Clock::now();
        Milliseconds ms_init = init_end - init_start;
        
        auto run_start = Clock::now();
        wfc.run("MRV");
        auto run_end = Clock::now();
        Milliseconds ms_run = run_end - run_start;
        
        std::cout << "Init: " << ms_init.count() << " ms, Run: " << ms_run.count() << " ms\n";
        
        if (generate_image) {
            ig.initialize(r, folder);
            ig.generate_image(wfc.matrix, output_file);
        }
    }
    else if (algorithm == "WFC_DIAGONAL") {
        WFC wfc;
        wfc.initialize_wfc(grid_size, grid_size, c, seed);
        auto init_end = Clock::now();
        Milliseconds ms_init = init_end - init_start;
        
        auto run_start = Clock::now();
        wfc.run("Diagonal");
        auto run_end = Clock::now();
        Milliseconds ms_run = run_end - run_start;
        
        std::cout << "Init: " << ms_init.count() << " ms, Run: " << ms_run.count() << " ms\n";
        
        if (generate_image) {
            ig.initialize(r, folder);
            ig.generate_image(wfc.matrix, output_file);
        }
    }
    else if (algorithm == "NWFC") {
        NWFC nwfc;
        nwfc.initialize_nwfc(grid_size, grid_size, subgrid_size, c, seed);
        auto init_end = Clock::now();
        Milliseconds ms_init = init_end - init_start;
        
        auto run_start = Clock::now();
        nwfc.run();
        auto run_end = Clock::now();
        Milliseconds ms_run = run_end - run_start;
        
        std::cout << "Init: " << ms_init.count() << " ms, Run: " << ms_run.count() << " ms\n";
        
        if (generate_image) {
            ig.initialize(r, folder);
            ig.generate_image(nwfc.matrix, output_file);
        }
    }
    else {
        std::cout << "Error: Unknown algorithm '" << algorithm << "'\n";
        print_usage(argv[0]);
        return 1;
    }
    
    t_end = Clock::now();
    Milliseconds ms_total = t_end - t_start;
    
    std::cout << "Read constraints: " << ms_read.count() << " ms\n";
    std::cout << "Total execution: " << ms_total.count() << " ms\n";
    
    if (generate_image) {
        std::cout << "Image saved to: " << output_file << std::endl;
    }

    return 0;
}