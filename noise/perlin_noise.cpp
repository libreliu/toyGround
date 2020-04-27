#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <memory>
#include <vec.h>


inline int hash_256(int m) {
    // 256 elem total
    const uint8_t pseudo_hash_arr[] = {151,160,137,91,90,15,
        131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
        190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
        88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,
        77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
        102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,
        135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,
        5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
        223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,
        129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
        251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
        49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
    };

    return pseudo_hash_arr[m & 0xFF];
}

/* Perlin Noise Generation 
 * - References:
 *   - https://blog.csdn.net/qq_34302921/article/details/80849139
 *   - https://zh.wikipedia.org/wiki/Perlin%E5%99%AA%E5%A3%B0
 *   - Perlin, K. (1985). An image synthesizer. 19(3), 287–296. 
 *     https://doi.org/10.1145/325334.325247
 *   - http://www.twinklingstar.cn/2015/2581/classical-perlin-noise/
 * 
 * Here I'm implementing the 2d version.
 */

class PerlinNoiseGenerator2D {
private:
    std::unique_ptr<vec2d[]> rand_G;
    int x_max;
    int y_max;
    int grid_max;

public:
    PerlinNoiseGenerator2D(int grid_max, int x_max, int y_max) :
        x_max(x_max), y_max(y_max), grid_max(grid_max) {
        // generate random gradients
        rand_G = std::make_unique<vec2d[]>(grid_max * grid_max);
        for (int i = 0; i < grid_max; i++) {
            rand_G[i] = vec2d(rand(), rand()).normalize();
        }
    }

    vec2d get_pseudo_grad(int m, int n) {
        // permutation array
        const unsigned char perm[] = {151,160,137,91,90,15,
            131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
            190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
            88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
            77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
            102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
            135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
            5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
            223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
            129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
            251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
            49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
            138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
        };
        assert(m <= 256 && n <= 256);

        return rand_G[ (m + perm[n]) % grid_max ];
    };

    inline double ease(double x) {
        return 3 * x * x - 2 * x * x * x;
    }

    double get(int x_in, int y_in) {
        // to grid, [0, grid_max]
        double x = (double)x_in / x_max * grid_max;
        double y = (double)y_in / y_max * grid_max;

        // grid base coord
        int x_0 = (int)x;
        int y_0 = (int)y;

        vec2d self(x, y);

        vec2d g00 = get_pseudo_grad(x_0, y_0);
        vec2d g01 = get_pseudo_grad(x_0, y_0 + 1);
        vec2d g10 = get_pseudo_grad(x_0 + 1, y_0);
        vec2d g11 = get_pseudo_grad(x_0 + 1, y_0 + 1);

        double s = g00.dot(self - vec2d(x_0, y_0));
        double t = g01.dot(self - vec2d(x_0 + 1, y_0));
        double u = g10.dot(self - vec2d(x_0, y_0 + 1));
        double v = g11.dot(self - vec2d(x_0 + 1, y_0 + 1));

        double a = s + (t - s) * ease(x - x_0);
        double b = u + (v - u) * ease(x - x_0);

        return a + (b - a) * ease(y - y_0);
    }

};


// double perlin_noise_2d(int x, int y) {
//     const int output_w = 1024;
//     const int output_h = 1024;
//     const int f_w = 1024;
//     const int f_h = 1024;
//     const double scale_w = (double) f_w / output_w;
//     const double scale_h = (double) f_h / output_h;

//     // [0, f_w]
//     double x_s = x * scale_w;
//     // [0, f_h]
//     double y_s = y * scale_h;

//     int x_int = (int)std::floor(x_s);
//     int y_int = (int)std::floor(y_s);
    
//     auto grad = [&](int x, int y) {
//         // [-1, 1]
//         double grad_x = ((double)hash_256(x) - 128) / 128;
//         double grad_y = ((double)hash_256(y) - 128) / 128;
//         return std::make_pair(grad_x, grad_y);
//     };

//     auto get_grid_noise = [&](int x, int y) {
//         auto grid_grad = grad(x, y);
//         return grid_grad.first * hash_256(x) 
//                 + grid_grad.second * hash_256(y);
//     };

//     // interpolate components
//     double xy = get_grid_noise(x, y);
//     double xyy = get_grid_noise(x, y + 1);
//     double xxy = get_grid_noise(x + 1, y);
//     double xxyy = get_grid_noise(x + 1, y + 1);

//     double dx = x_s - x_int;
//     double dy = y_s - y_int;

//     double dist_xy = dx * dx + dy * dy;
//     double dist_xyy = dx * dx + (1-dy) * (1-dy);
//     double dist_xxy = (1-dx) * (1-dx) + dy * dy;
//     double dist_xxyy = (1-dx) * (1-dx) + (1-dy) * (1-dy);

//     return dist_xy * xy + dist_xxy * xxy + dist_xyy * xyy + dist_xxyy * xxyy; 

// }

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

class UniformNoiseGenerator {
public:
    double get(int x, int y) {
        return (double)rand() / RAND_MAX;
    }
};

int main(void) {
    NoiseWriter<UniformNoiseGenerator> uniform("noise_uniform.png", 1024, 1024);
    uniform.run();

    NoiseWriter<PerlinNoiseGenerator2D> perlin("noise_perlin.png", 1024, 1024, 50, 1024, 1024);
    perlin.run();
}