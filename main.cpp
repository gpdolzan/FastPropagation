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
#include <iomanip>
#include <sstream>

// Helper function to format memory size with appropriate units
std::string format_memory_size(size_t bytes) {
    if (bytes >= 1024ULL * 1024 * 1024) { // >= 1 GB
        double gb_value = static_cast<double>(bytes) / (1024.0 * 1024 * 1024);
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << gb_value << " GB";
        return oss.str();
    } else if (bytes >= 1024 * 1024) { // >= 1 MB
        return std::to_string(bytes / (1024 * 1024)) + " MB";
    } else if (bytes >= 1024) { // >= 1 KB
        return std::to_string(bytes / 1024) + " KB";
    } else {
        return std::to_string(bytes) + " bytes";
    }
}

void print_usage(const char* program_name) {
    std::cout << "Argumentos:\n";
    std::cout << "  algoritmo: FP, FP_BACKTRACK, FP_DIAGONAL, FP_DIAGONAL_BACKTRACK, WFC, WFC_BACKTRACK, WFC_DIAGONAL, WFC_DIAGONAL_BACKTRACK, NWFC, NWFC_BACKTRACK\n";
    std::cout << "  pasta: Tilesets -> Roads, Raods--, Roads++, Carcassonne, Carcassonne++\n";
    std::cout << "  grid_size: Size of the grid (e.g., 10 for 10x10)\n";
    std::cout << "  seed: Random seed (integer)\n";
    std::cout << "  gerar_imagem: 1 gera iamgem, 0 nao gera\n";
    std::cout << "  num_runs: number of times to run the algorithm\n";
    std::cout << "  subgrid_size: necessário se for usar o NWFC onde o tamanho_subgrid >= 2\n";
    std::cout << "Usage: main <algoritmo> <pasta> <tamanho_matriz> <seed> <gerar_imagem> <num_runs> [tamanho_subgrid]\n";
    std::cout << "Exemplos:\n";
    std::cout << "main WFC Roads 10 1234 1 5\n";
    std::cout << "main WFC_BACKTRACK Roads 10 1234 1 3\n";
    std::cout << "main WFC_DIAGONAL Roads 10 1234 1 3\n";
    std::cout << "main WFC_DIAGONAL_BACKTRACK Roads 10 1234 1 3\n";
    std::cout << "main FP_BACKTRACK Roads 10 1234 1 3\n";
    std::cout << "main FP_DIAGONAL_BACKTRACK Roads 10 1234 1 3\n";
    std::cout << "main NWFC Assets 20 5678 0 10 3\n";
    std::cout << "main NWFC_BACKTRACK Assets 20 5678 0 10 3\n";
    std::cout << "main FP_DIAGONAL Roads++ 15 9999 1 3\n";
}

int main(int argc, char const *argv[])
{
    if (argc < 7) {
        print_usage(argv[0]);
        return 1;
    }

    // Parse command line arguments
    std::string algorithm = argv[1];
    std::string folder = argv[2];
    int grid_size = std::stoi(argv[3]);
    int seed = std::stoi(argv[4]);
    bool generate_image = (std::stoi(argv[5]) == 1);
    int num_runs = std::stoi(argv[6]);
    int subgrid_size = 2; // default
    
    if (algorithm == "NWFC" || algorithm == "NWFC_BACKTRACK") {
        if (argc < 8) {
            std::cout << "Error: NWFC and NWFC_BACKTRACK require subgrid_size parameter\n";
            print_usage(argv[0]);
            return 1;
        }
        subgrid_size = std::stoi(argv[7]);
    }

    // Fixed parameters
    std::string output_file = algorithm + "_" + folder + "_" + std::to_string(grid_size) + "_" + std::to_string(seed) + ".png";

    std::cout << "Running " << algorithm << " on " << grid_size << "x" << grid_size 
              << " grid with tileset '" << folder << "' and seed " << seed << " for " << num_runs << " runs";
    if (algorithm == "NWFC" || algorithm == "NWFC_BACKTRACK") {
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

    // Variables to accumulate times
    double total_init_time = 0.0;
    double total_run_time = 0.0;
    double total_execution_time = 0.0;
    int total_backtracks = 0;
    size_t total_backtrack_memory_cost = 0;

    // Run the algorithm multiple times
    for (int run = 0; run < num_runs; run++) {
        std::cout << "Run " << (run + 1) << "/" << num_runs << "..." << std::endl;
        
        // Initialize and run algorithm
        t_start = Clock::now();
        auto init_start = Clock::now();
        
        if (algorithm == "FP") {
            FastPropagation fp;
            fp.initialize_fp(grid_size, grid_size, c, seed + run); // Use different seed for each run
            auto init_end = Clock::now();
            Milliseconds ms_init = init_end - init_start;
            
            auto run_start = Clock::now();
            fp.run("FP");
            auto run_end = Clock::now();
            Milliseconds ms_run = run_end - run_start;
            
            total_init_time += ms_init.count();
            total_run_time += ms_run.count();
            
            // Display memory usage for first run
            if (run == 0) {
                size_t memory_total = fp.get_memory_usage();
                std::cout << "  Memory usage: " << format_memory_size(memory_total) << std::endl;
            }
            
            if (generate_image && run == 0) { // Only generate image for first run
                ig.initialize(r, folder);
                ig.generate_image(fp.matrix, output_file);
            }
        }
        else if (algorithm == "FP_BACKTRACK") {
            FastPropagation fp;
            fp.initialize_fp(grid_size, grid_size, c, seed + run);
            auto init_end = Clock::now();
            Milliseconds ms_init = init_end - init_start;
            
            auto run_start = Clock::now();
            fp.FP(true); // Enable backtracking
            auto run_end = Clock::now();
            Milliseconds ms_run = run_end - run_start;
            
            total_init_time += ms_init.count();
            total_run_time += ms_run.count();
            
            int run_backtracks = fp.get_backtrack_count();
            size_t run_backtrack_memory = fp.get_backtrack_stack_memory_usage(); // Use current stack size instead of cumulative cost
            
            total_backtracks += run_backtracks;
            total_backtrack_memory_cost += run_backtrack_memory;
            
            std::cout << "  Backtracks: " << run_backtracks << ", Stack memory: " << format_memory_size(run_backtrack_memory) << std::endl;
            
            // Display memory usage for first run
            if (run == 0) {
                size_t memory_total = fp.get_memory_usage();
                std::cout << "  Total memory usage: " << format_memory_size(memory_total) << std::endl;
            }
            
            if (generate_image && run == 0) {
                ig.initialize(r, folder);
                ig.generate_image(fp.matrix, output_file);
            }
        }
        else if (algorithm == "FP_DIAGONAL") {
            FastPropagation fp;
            fp.initialize_fp(grid_size, grid_size, c, seed + run);
            auto init_end = Clock::now();
            Milliseconds ms_init = init_end - init_start;
            
            auto run_start = Clock::now();
            fp.run("Diagonal");
            auto run_end = Clock::now();
            Milliseconds ms_run = run_end - run_start;
            
            total_init_time += ms_init.count();
            total_run_time += ms_run.count();
            
            // Display memory usage for first run
            if (run == 0) {
                size_t memory_total = fp.get_memory_usage();
                std::cout << "  Memory usage: " << format_memory_size(memory_total) << std::endl;
            }
            
            if (generate_image && run == 0) {
                ig.initialize(r, folder);
                ig.generate_image(fp.matrix, output_file);
            }
        }
        else if (algorithm == "FP_DIAGONAL_BACKTRACK") {
            FastPropagation fp;
            fp.initialize_fp(grid_size, grid_size, c, seed + run);
            auto init_end = Clock::now();
            Milliseconds ms_init = init_end - init_start;
            
            auto run_start = Clock::now();
            fp.Diag(true); // Enable backtracking for diagonal
            auto run_end = Clock::now();
            Milliseconds ms_run = run_end - run_start;
            
            total_init_time += ms_init.count();
            total_run_time += ms_run.count();
            
            int run_backtracks = fp.get_backtrack_count();
            size_t run_backtrack_memory = fp.get_backtrack_stack_memory_usage(); // Use current stack size instead of cumulative cost
            
            total_backtracks += run_backtracks;
            total_backtrack_memory_cost += run_backtrack_memory;
            
            std::cout << "  Backtracks: " << run_backtracks << ", Stack memory: " << format_memory_size(run_backtrack_memory) << std::endl;
            
            // Display memory usage for first run
            if (run == 0) {
                size_t memory_total = fp.get_memory_usage();
                std::cout << "  Total memory usage: " << format_memory_size(memory_total) << std::endl;
            }
            
            if (generate_image && run == 0) {
                ig.initialize(r, folder);
                ig.generate_image(fp.matrix, output_file);
            }
        }
        else if (algorithm == "WFC") {
            WFC wfc;
            wfc.initialize_wfc(grid_size, grid_size, c, seed + run);
            auto init_end = Clock::now();
            Milliseconds ms_init = init_end - init_start;
            
            auto run_start = Clock::now();
            wfc.run("MRV");
            auto run_end = Clock::now();
            Milliseconds ms_run = run_end - run_start;
            
            total_init_time += ms_init.count();
            total_run_time += ms_run.count();
            
            // Display memory usage for first run
            if (run == 0) {
                size_t memory_total = wfc.get_memory_usage();
                std::cout << "  Memory usage: " << format_memory_size(memory_total) << std::endl;
            }
            
            if (generate_image && run == 0) {
                ig.initialize(r, folder);
                ig.generate_image(wfc.matrix, output_file);
            }
        }
        else if (algorithm == "WFC_BACKTRACK") {
            WFC wfc;
            wfc.initialize_wfc(grid_size, grid_size, c, seed + run);
            auto init_end = Clock::now();
            Milliseconds ms_init = init_end - init_start;
            
            auto run_start = Clock::now();
            wfc.MRV(true); // Enable backtracking
            auto run_end = Clock::now();
            Milliseconds ms_run = run_end - run_start;
            
            total_init_time += ms_init.count();
            total_run_time += ms_run.count();
            
            int run_backtracks = wfc.get_backtrack_count();
            size_t run_backtrack_memory = wfc.get_backtrack_stack_memory_usage();
            
            total_backtracks += run_backtracks;
            total_backtrack_memory_cost += run_backtrack_memory;
            
            std::cout << "  Backtracks: " << run_backtracks << ", Stack memory: " << format_memory_size(run_backtrack_memory) << std::endl;
            
            // Display memory usage for first run
            if (run == 0) {
                size_t memory_total = wfc.get_memory_usage();
                std::cout << "  Total memory usage: " << format_memory_size(memory_total) << std::endl;
            }
            
            if (generate_image && run == 0) {
                ig.initialize(r, folder);
                ig.generate_image(wfc.matrix, output_file);
            }
        }
        else if (algorithm == "WFC_DIAGONAL") {
            WFC wfc;
            wfc.initialize_wfc(grid_size, grid_size, c, seed + run);
            auto init_end = Clock::now();
            Milliseconds ms_init = init_end - init_start;
            
            auto run_start = Clock::now();
            wfc.run("Diagonal");
            auto run_end = Clock::now();
            Milliseconds ms_run = run_end - run_start;
            
            total_init_time += ms_init.count();
            total_run_time += ms_run.count();
            
            // Display memory usage for first run
            if (run == 0) {
                size_t memory_total = wfc.get_memory_usage();
                std::cout << "  Memory usage: " << format_memory_size(memory_total) << std::endl;
            }
            
            if (generate_image && run == 0) {
                ig.initialize(r, folder);
                ig.generate_image(wfc.matrix, output_file);
            }
        }
        else if (algorithm == "WFC_DIAGONAL_BACKTRACK") {
            WFC wfc;
            wfc.initialize_wfc(grid_size, grid_size, c, seed + run);
            auto init_end = Clock::now();
            Milliseconds ms_init = init_end - init_start;
            
            auto run_start = Clock::now();
            wfc.Diag(true); // Enable backtracking for diagonal
            auto run_end = Clock::now();
            Milliseconds ms_run = run_end - run_start;
            
            total_init_time += ms_init.count();
            total_run_time += ms_run.count();
            
            int run_backtracks = wfc.get_backtrack_count();
            size_t run_backtrack_memory = wfc.get_backtrack_stack_memory_usage();
            
            total_backtracks += run_backtracks;
            total_backtrack_memory_cost += run_backtrack_memory;
            
            std::cout << "  Backtracks: " << run_backtracks << ", Stack memory: " << format_memory_size(run_backtrack_memory) << std::endl;
            
            // Display memory usage for first run
            if (run == 0) {
                size_t memory_total = wfc.get_memory_usage();
                std::cout << "  Total memory usage: " << format_memory_size(memory_total) << std::endl;
            }
            
            if (generate_image && run == 0) {
                ig.initialize(r, folder);
                ig.generate_image(wfc.matrix, output_file);
            }
        }
        else if (algorithm == "NWFC") {
            NWFC nwfc;
            nwfc.initialize_nwfc(grid_size, grid_size, subgrid_size, c, seed + run);
            auto init_end = Clock::now();
            Milliseconds ms_init = init_end - init_start;
            
            auto run_start = Clock::now();
            nwfc.run();
            auto run_end = Clock::now();
            Milliseconds ms_run = run_end - run_start;
            
            total_init_time += ms_init.count();
            total_run_time += ms_run.count();
            
            // Display memory usage for first run
            if (run == 0) {
                size_t memory_total = nwfc.get_memory_usage();
                std::cout << "  Memory usage: " << format_memory_size(memory_total) << std::endl;
            }
            
            if (generate_image && run == 0) {
                ig.initialize(r, folder);
                ig.generate_image(nwfc.matrix, output_file);
            }
        }
        else if (algorithm == "NWFC_BACKTRACK") {
            NWFC nwfc;
            nwfc.initialize_nwfc(grid_size, grid_size, subgrid_size, c, seed + run);
            auto init_end = Clock::now();
            Milliseconds ms_init = init_end - init_start;
            
            auto run_start = Clock::now();
            nwfc.run(true); // Enable backtracking
            auto run_end = Clock::now();
            Milliseconds ms_run = run_end - run_start;
            
            total_init_time += ms_init.count();
            total_run_time += ms_run.count();
            
            int run_backtracks = nwfc.get_total_backtrack_count();
            size_t run_backtrack_memory = nwfc.get_total_backtrack_stack_memory_usage();
            
            total_backtracks += run_backtracks;
            total_backtrack_memory_cost += run_backtrack_memory;
            
            std::cout << "  Backtracks: " << run_backtracks << ", Stack memory: " << format_memory_size(run_backtrack_memory) << std::endl;
            
            // Display memory usage for first run
            if (run == 0) {
                size_t memory_total = nwfc.get_memory_usage();
                std::cout << "  Total memory usage: " << format_memory_size(memory_total) << std::endl;
            }
            
            if (generate_image && run == 0) {
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
        total_execution_time += ms_total.count();
    }
    
    // Calculate averages
    double avg_init_time = total_init_time / num_runs;
    double avg_run_time = total_run_time / num_runs;
    double avg_total_time = total_execution_time / num_runs;
    
    std::cout << "\n=== RESULTS SUMMARY ===\n";
    std::cout << "Number of runs: " << num_runs << "\n";
    std::cout << "Read constraints: " << ms_read.count() << " ms\n";
    std::cout << "Average init time: " << avg_init_time << " ms\n";
    std::cout << "Average run time: " << avg_run_time << " ms\n";
    std::cout << "Average total execution time: " << avg_total_time << " ms\n";
    std::cout << "Total init time: " << total_init_time << " ms\n";
    std::cout << "Total run time: " << total_run_time << " ms\n";
    std::cout << "Total execution time: " << total_execution_time << " ms\n";
    
    // Display backtrack statistics for backtracking algorithms
    if (algorithm == "FP_BACKTRACK" || algorithm == "FP_DIAGONAL_BACKTRACK" || 
        algorithm == "WFC_BACKTRACK" || algorithm == "WFC_DIAGONAL_BACKTRACK" ||
        algorithm == "NWFC_BACKTRACK") {
        double avg_backtracks = static_cast<double>(total_backtracks) / num_runs;
        double avg_backtrack_memory = static_cast<double>(total_backtrack_memory_cost) / num_runs;
        
        std::cout << "=== BACKTRACK STATISTICS ===\n";
        std::cout << "Total backtracks: " << total_backtracks << "\n";
        std::cout << "Average backtracks per run: " << std::fixed << std::setprecision(1) << avg_backtracks << "\n";
        std::cout << "Total backtrack stack memory: " << format_memory_size(total_backtrack_memory_cost) << "\n";
        std::cout << "Average backtrack stack memory per run: " << format_memory_size(static_cast<size_t>(avg_backtrack_memory)) << "\n";
    }
    
    if (generate_image) {
        std::cout << "Image saved to: " << output_file << " (from first run)" << std::endl;
    }

    return 0;
}