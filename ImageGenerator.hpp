#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Matrix.hpp"
#include "Reader.hpp"
#include "stb_image.h"
#include "stb_image_write.h"

class ImageGenerator
{
private:
    std::map<int, std::string> tile_id_to_filename;
    std::string tiles_folder_path;
    int tile_width;
    int tile_height;
    int channels;

public:
    void initialize(const Reader& reader, const std::string& folder_path);
    void generate_image(const Matrix& matrix, const std::string& output_filename);
    ImageGenerator();
    ~ImageGenerator();
};
