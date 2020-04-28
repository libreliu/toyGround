#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <memory>
#include <vec.h>
#include <vector>
#include <array>
#include <map>

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
    int grid_max;

public:
    PerlinNoiseGenerator2D(int grid_max) : grid_max(grid_max) {
        // generate random gradients
        rand_G = std::make_unique<vec2d[]>(grid_max * grid_max);
        for (int i = 0; i < grid_max * grid_max; i++) {
            rand_G[i] = vec2d(rand() - (RAND_MAX / 2), rand() - (RAND_MAX / 2)).normalize();
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

        return rand_G[ (m + perm[n]) % (grid_max * grid_max) ];
    };

    // inline double ease(double x) {
    //     return 3 * x * x - 2 * x * x * x;
    // }

    inline double ease(double x) {
        return x * x * x * (x * (x * 6 - 15) + 10);
    }

    double get(int x_in, int y_in) {
        // to grid
        double x = (double)x_in / grid_max;
        double y = (double)y_in / grid_max;

        // grid base coord
        int x_0 = (int)std::floor(x);
        int y_0 = (int)std::floor(y);

        vec2d self(x, y);

        vec2d g00 = get_pseudo_grad(x_0, y_0);
        vec2d g01 = get_pseudo_grad(x_0, y_0 + 1);
        vec2d g10 = get_pseudo_grad(x_0 + 1, y_0);
        vec2d g11 = get_pseudo_grad(x_0 + 1, y_0 + 1);

        double s = g00.dot(vec2d(x - x_0, y - y_0));
        // This will cause discontinuity!
        // double t = g10.dot(vec2d(x_0 - x + 1, y - y_0));
        // double u = g01.dot(vec2d(x - x_0, y_0 - y + 1));
        // double v = g11.dot(vec2d(x_0 - x + 1, y_0 - y + 1));
        double t = g10.dot(vec2d(x - x_0 - 1, y - y_0));
        double u = g01.dot(vec2d(x - x_0, y - y_0 - 1));
        double v = g11.dot(vec2d(x - x_0 - 1, y - y_0 - 1));

        double a = s + (t - s) * ease(x - x_0);
        double b = u + (v - u) * ease(x - x_0);

        return a + (b - a) * ease(y - y_0);
    }

};

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

/* A simple worley noise generator.
 * 
 * This generator tries to achieve even distribution in given window.
 * - Window size: x_max, y_max
 * - Total points estimation: n_total
 *   - This is not the actual point it'll eventually make, but
 *     this will influence the density (points / (pixel^2))
 * - Grid size: grid_max
 *   - For worley, there must be points in neighbouring grid, and
 *     setting this too small will make n_total impossible
 *     (and an assertion raises)
 * - Max points per grid: grid_point_max
 *   - setting this too low will affect the accuracy of n_total
 *   - but too high decreases speed
 */

class WorleyNoiseGenerator {
public:
    double get(int x_in, int y_in) {
        // convert to grid coord
        double x = (double)x_in / grid_max;
        double y = (double)y_in / grid_max;

        vec2d self(x, y);

        int x_0 = (int)std::floor(x);
        int y_0 = (int)std::floor(x);

        // find nearest neighbor point from 9-way
        // - though 25 lattice is in fact needed
        std::array<vec2i, 9> possible_grid = {
            vec2i(x_0-1, y_0+1), vec2i(x_0, y_0+1), vec2i(x_0+1, y_0), 
            vec2i(x_0-1, y_0)  , vec2i(x_0, y_0)  , vec2i(x_0+1, y_0), 
            vec2i(x_0-1, y_0-1), vec2i(x_0, y_0-1), vec2i(x_0+1, y_0-1)
        };

        // 2 neighbor for now
        // - also works best, I think
        std::array<vec2i, 2> dist_points = {
            vec2i(0, 0),  vec2i(0, 0)
        };

        std::array<vec2i, 2> dist_base = {
            vec2i(0, 0),  vec2i(0, 0)
        };

        std::array<double, 2> dist_array = {
            1e10, 1e10
        };

        for (auto &grid_coord : possible_grid) {
            // get all points in grid
            auto points = get_grid_points(grid_coord.x(), grid_coord.y());
            for (auto &point_int : points) {
                // get the actual position of the point
                vec2d point = point_int.cast_to<double>() / (double)hash_max;
                vec2d actual = point * grid_max + grid_coord.cast_to<double>();
                double curr_dist = (self - actual).squared_norm();
                if (curr_dist < dist_array[0]) {
                    dist_points[1] = dist_points[0];
                    dist_array[1] = dist_array[0];
                    dist_base[1] = dist_base[0];

                    dist_points[0] = point_int;
                    dist_array[0] = curr_dist;
                    dist_base[0] = grid_coord;
                } else if (curr_dist < dist_array[1]) {
                    dist_points[1] = point_int;
                    dist_array[1] = curr_dist;
                    dist_base[1] = grid_coord;
                }
            }
        }

        assert(dist_array[0] < 1e10 && dist_array[1] < 1e10);

        double weight_sum = dist_array[0] + dist_array[1];
        std::array<double, 2> weight_array = {
            dist_array[0] / weight_sum, dist_array[1] / weight_sum
        };

        return get_point_intensity(dist_base[0], dist_points[0]);
    }

    WorleyNoiseGenerator(int grid_max, double prob_next, int grid_point_max) {
        this->hash_max = RAND_MAX;
        this->grid_max = grid_max;
        this->prob_next = prob_next;
        this->grid_point_max = grid_point_max;

        // print all lattice points
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                auto points_int = get_grid_points(i, j);
                for (auto &point_int : points_int) {
                    vec2d point = point_int.cast_to<double>() / (double)hash_max;
                    vec2d actual = point * grid_max + vec2i(i, j).cast_to<double>();

                    printf("%lf,%lf,%lf\n", actual.x(), actual.y()
                            , get_point_intensity(vec2i(i,j), point_int));
                }
            }
        }
    }

private:
    
    // maximum value a hash shall return
    int hash_max;
    int grid_max;
    double prob_next;
    int grid_point_max;

    std::map<int, int> hash_map;

    // a simple hash, used to generate some randomness
    int hash(int m) {
        // srand(m);
        // return rand();
        
        auto res = hash_map.find(m);
        if (res == hash_map.end()) {
            int ret = rand();
            hash_map.insert(std::make_pair(m, ret));
            return ret;
        } else {
            return res->second;
        }
    }

    // ! This should be stable locally, to avoid errors caused by trunctuation
    // So I've made this accepting integer only
    double get_point_intensity(vec2i base, vec2i point) {
        return (double)
                hash(
                    hash(
                        hash(
                            base.x() + hash(base.y())
                        ) + point.x()
                    ) + point.y()
                ) / hash_max;
    }

    // [0, 1]
    // ! FP may not have the stability required
    double n_hash(int m) {
        int res = hash(m);
        return (double)res / hash_max;
    }

    /* - returns
     * 1. a point (in [0, 1]^2 when regulated)
     * 2. a seed for next iteration
     * 3. whether we're going to have the next point
     */
    vec2i get_point(int m, double prob_next, bool &next_iter, int &seed) {
        int x_int = hash(m);
        int y_int = hash(x_int);

        double x = (double)x_int / hash_max;
        double y = (double)y_int / hash_max;

        assert(y_int <= hash_max && x_int <= hash_max);

        // [0, prob_next]: we can proceed to next iteration
        if (y < prob_next) {
            next_iter = true;
        } else {
            next_iter = false;
        }

        seed = y_int;

        return vec2i(x_int, y_int);
    };

    /* Gen 1st point with 1 probability
     * Then, gen next point with prob_next probability
     * and Est.(num_of_points) = 1 / (1-prob_next)^2
     */
    std::vector<vec2i> get_grid_points(int grid_x, int grid_y) {
        bool next_iter = false;
        int seed = 0;
        std::vector<vec2i> res;
        res.reserve(grid_point_max);

        // generate first point
        vec2i point = get_point(hash(hash(grid_x) + grid_y), prob_next, next_iter, seed);
        res.push_back(point);

        while (next_iter && res.size() < grid_point_max) {
            vec2i point = get_point(seed, prob_next, next_iter, seed);
            res.push_back(point);
        }

        return res;
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

    NoiseWriter<PerlinNoiseGenerator2D> perlin("noise_perlin.png", 1024, 1024, 200);
    perlin.run();

    NoiseWriter<WorleyNoiseGenerator> worley("noise_worley.png", 500, 500, 50, 0.3, 5);
    worley.run();
}