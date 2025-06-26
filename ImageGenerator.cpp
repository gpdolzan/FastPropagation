#include "ImageGenerator.hpp"

void ImageGenerator::initialize(const Reader& reader, const std::string& folder_path)
{
    tiles_folder_path = folder_path;
    tile_width = 0;
    tile_height = 0;
    channels = 0;

    // Create mapping from tile ID to filename
    for (int i = 0; i < reader.constraints.size(); i++)
    {
        tile_id_to_filename[i] = reader.constraints[i] + ".png";
    }

    // Load first tile to get dimensions
    if (!reader.constraints.empty())
    {
        std::string first_tile_path = folder_path + "/" + tile_id_to_filename[0];
        unsigned char* data = stbi_load(first_tile_path.c_str(), &tile_width, &tile_height, &channels, 0);
        if (data)
        {
            stbi_image_free(data);
            std::cout << "Tile dimensions: " << tile_width << "x" << tile_height << " with " << channels << " channels" << std::endl;
        }
        else
        {
            std::cerr << "Error: Could not load tile image: " << first_tile_path << std::endl;
        }
    }
}

void ImageGenerator::generate_image(const Matrix& matrix, const std::string& output_filename)
{
    if (tile_width == 0 || tile_height == 0)
    {
        std::cerr << "Error: ImageGenerator not properly initialized!" << std::endl;
        return;
    }

    int output_width = matrix.columns * tile_width;
    int output_height = matrix.rows * tile_height;
    
    // Check if the output image would be too large (more than 1GB)
    long long total_pixels = (long long)output_width * output_height;
    long long total_bytes = total_pixels * channels;
    const long long max_bytes = 1LL * 1024 * 1024 * 1024; // 1GB limit
    
    if (total_bytes > max_bytes)
    {
        std::cerr << "Error: Output image would be too large (" 
                  << total_bytes / (1024 * 1024) << " MB). "
                  << "Consider reducing matrix size or tile dimensions." << std::endl;
        std::cerr << "Current dimensions: " << output_width << "x" << output_height 
                  << " pixels (" << matrix.rows << "x" << matrix.columns 
                  << " tiles of " << tile_width << "x" << tile_height << " pixels each)" << std::endl;
        return;
    }
    
    std::cout << "Creating output image: " << output_width << "x" << output_height 
              << " pixels (" << total_bytes / (1024 * 1024) << " MB)" << std::endl;
    
    // Create output image buffer
    std::vector<unsigned char> output_image;
    try {
        output_image.resize(output_width * output_height * channels, 0);
    } catch (const std::exception& e) {
        std::cerr << "Error: Could not allocate memory for output image: " << e.what() << std::endl;
        return;
    }

    // Process each cell in the matrix
    for (int row = 0; row < matrix.rows; row++)
    {
        for (int col = 0; col < matrix.columns; col++)
        {
            int tile_id = matrix.matrix[row][col].collapsed;
            
            if (tile_id == -1)
            {
                // Cell not collapsed, fill with black
                for (int y = 0; y < tile_height; y++)
                {
                    for (int x = 0; x < tile_width; x++)
                    {
                        int output_y = row * tile_height + y;
                        int output_x = col * tile_width + x;
                        int output_index = (output_y * output_width + output_x) * channels;
                        
                        for (int c = 0; c < channels; c++)
                        {
                            output_image[output_index + c] = 0;
                        }
                    }
                }
                continue;
            }

            // Load the tile image
            auto it = tile_id_to_filename.find(tile_id);
            if (it == tile_id_to_filename.end())
            {
                std::cerr << "Warning: Unknown tile ID " << tile_id << " at position (" << row << ", " << col << ")" << std::endl;
                continue;
            }

            std::string tile_path = tiles_folder_path + "/" + it->second;
            int loaded_width, loaded_height, loaded_channels;
            unsigned char* tile_data = stbi_load(tile_path.c_str(), &loaded_width, &loaded_height, &loaded_channels, channels);
            
            if (!tile_data)
            {
                std::cerr << "Warning: Could not load tile image: " << tile_path << std::endl;
                continue;
            }

            // Copy tile data to output image
            for (int y = 0; y < tile_height && y < loaded_height; y++)
            {
                for (int x = 0; x < tile_width && x < loaded_width; x++)
                {
                    int output_y = row * tile_height + y;
                    int output_x = col * tile_width + x;
                    int output_index = (output_y * output_width + output_x) * channels;
                    int tile_index = (y * loaded_width + x) * channels;
                    
                    for (int c = 0; c < channels; c++)
                    {
                        output_image[output_index + c] = tile_data[tile_index + c];
                    }
                }
            }

            stbi_image_free(tile_data);
        }
    }

    // Write the output image
    int success = 0;
    if (output_filename.find(".png") != std::string::npos)
    {
        success = stbi_write_png(output_filename.c_str(), output_width, output_height, channels, output_image.data(), output_width * channels);
    }
    else if (output_filename.find(".jpg") != std::string::npos || output_filename.find(".jpeg") != std::string::npos)
    {
        success = stbi_write_jpg(output_filename.c_str(), output_width, output_height, channels, output_image.data(), 90);
    }
    else
    {
        // Default to PNG
        std::string png_filename = output_filename + ".png";
        success = stbi_write_png(png_filename.c_str(), output_width, output_height, channels, output_image.data(), output_width * channels);
    }

    if (success)
    {
        std::cout << "Successfully generated image: " << output_filename << " (" << output_width << "x" << output_height << ")" << std::endl;
    }
    else
    {
        std::cerr << "Error: Failed to write image: " << output_filename << std::endl;
    }
}

ImageGenerator::ImageGenerator()
{
    tile_width = 0;
    tile_height = 0;
    channels = 0;
}

ImageGenerator::~ImageGenerator()
{
}
