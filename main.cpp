#include "Reader.hpp"
#include "Tile.hpp"
#include "Cell.hpp"
#include "FastPropagation.hpp"
#include "ImageGenerator.hpp"
#include "WFC.hpp"
#include <chrono>

int main(int argc, char const *argv[])
{
    // MUDAR PARAMETROS AQUI
    std::string folder = "Roads++";
    int width = 100;
    int height = 100;
    int seed = 2006;
    std::string output_file = "output.png";

    Reader r;
    Cell c;
    Matrix m;
    FastPropagation fp;
    WFC wfc;
    ImageGenerator ig;

    // Chrono
    using Clock = std::chrono::high_resolution_clock;
    using Milliseconds = std::chrono::duration<double, std::milli>;

    // Leitura dos constraints
    auto t_start = Clock::now();
    r.read_files(folder); // Reads all the constraints
    c.domain = r.generate_domain();
    auto t_end = Clock::now();
    Milliseconds ms_read = t_end - t_start;

    // Inicializacao da matriz
    t_start = Clock::now();
    fp.initialize_fp(width, height, c, seed);
    //wfc.initialize_wfc(width, height, c, seed);
    t_end = Clock::now();
    Milliseconds ms_init = t_end - t_start;
    
    // Propagação Rápida
    t_start = Clock::now();
    fp.run("FP");
    //wfc.run("Diagonal");
    t_end = Clock::now();
    Milliseconds ms = t_end - t_start;


    std::cout << "Time to Read Constraints: " << ms_read.count() << " ms\n";
    std::cout << "Time to initialize: " << ms_init.count() << " ms\n";
    std::cout << "Time to run: " << ms.count() << " ms\n";
    std::cout << "Total time: " << ms_read.count() + ms_init.count() + ms.count() << " ms\n";

    // std::cout << "\nGenerating output image..." << std::endl;
    // ig.initialize(r, folder);
    // ig.generate_image(fp.matrix, output_file);
    // ig.generate_image(wfc.matrix, output_file);

    return 0;
}