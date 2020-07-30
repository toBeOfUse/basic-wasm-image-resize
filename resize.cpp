#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
using namespace emscripten;

#include <stdio.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

static void my_progress_report(float progress){
    std::cout << "progress: " << progress << std::endl << std::flush;
}
#define STBIR_PROGRESS_REPORT(val) my_progress_report(val)

std::string m_resize(std::string filename, int newWidth, int newHeight)
{
    std::cout << "started\n";
    int x, y, n;
    const unsigned char *data = stbi_load(filename.c_str(), &x, &y, &n, 0);
    std::cout << "loaded\n";
    const double ar = (float)x/y;

    if (newWidth == 0){
        newWidth = newHeight*ar;
    } else if (newHeight == 0){
        newHeight = newWidth/ar;
    }

    std::cout << "resizing to " << newWidth << " x " << newHeight << " with " << n << " channels" << std::endl << std::flush;

    unsigned char * resized_data = (unsigned char *)malloc(newWidth*newHeight*n);
    int success = stbir_resize_uint8(data, x, y, 0, resized_data, newWidth, newHeight, 0, n);
    std::cout << success << std::endl;
    
    const int dotPos = filename.find_last_of(".");
    const std::string name = filename.substr(0, dotPos);
    const std::string type = filename.substr(dotPos+1);
    std::string newFilename;
    
    if (type == "jpg" || type == "jpeg"){
        newFilename = name+"_r.jpg";
        std::cout << "storing at " << newFilename << std::endl;
        stbi_write_jpg(newFilename.c_str(), newWidth, newHeight, n, (void *)resized_data, 90);
    } else if (type == "png") {
        newFilename = name+"_r.png";
        std::cout << "storing at " << newFilename << std::endl;
        stbi_write_png(newFilename.c_str(), newWidth, newHeight, n, (void *)resized_data, 0);
    }

    std::cout << "complete" << std::endl;

    return newFilename;
}

EMSCRIPTEN_BINDINGS(resizer)
{
    function<std::string, std::string, int, int>("resize", &m_resize);
}
