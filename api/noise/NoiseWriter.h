#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <memory>
#include <vec.h>
#include <vector>
#include <array>
#include <map>

template <typename T>
class NoiseWriter {
private:
    T *generator;
    std::string filename;
    int width;
    int height;
public:

    template <typename... U>
    NoiseWriter(const char *filename_, int width, int height, U&&... u) :
      width(width), height(height) {
        generator = new T(std::forward<U>(u)...);
        filename = filename_;
    }

    ~NoiseWriter() {
        delete generator;
    }

    void run() {
        double* noise_data = new double[width * height];
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                noise_data[i * height + j] = generator->get(i, j);
            }
        }

        uint8_t* stbi_data = new uint8_t[width * height];
        for (size_t i = 0; i < width * height; i++)
            stbi_data[i] = static_cast<unsigned char>(std::clamp(noise_data[i] * 255, 0.0, 255.0));
        stbi_write_png(filename.c_str(), width, height, 1, stbi_data, width);
        delete [] noise_data;
        delete [] stbi_data;
    }
};