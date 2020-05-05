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
    double get(int x_in, int y_in);

    WorleyNoiseGenerator(int grid_max, double prob_next, int grid_point_max, int seed); 

private:
    
    // maximum value a hash shall return
    int hash_max;
    int grid_max;
    double prob_next;
    int grid_point_max;
    int seed;

    std::map<int, int> hash_map;

    // x = 0 -> 0
    // x = 1 -> 1
    inline double ease(double x) {
        return x * x * x * (x * (x * 6 - 15) + 10);
    }

    inline double sorder_ease(double x, double coeff) {
        //return - coeff * x + 1;
        return 1 - 0.005972 * x - 0.000451 * x * x + 0.0000135 * x * x * x - 2.025e-7 * x * x * x * x;
        //return 1 - 0.01315661 * x + 0.0008915745 * x * x - 0.00002734948 * x * x * x + 1.33317e-7 * x * x * x * x;
    }

    // a simple hash, used to generate some randomness
    inline int hash(int m) {
        
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
        
        return perm[(m * 48271 + seed) % 256];
    }

    // ! This should be stable locally, to avoid errors caused by trunctuation
    // So I've made this accepting integer only
    inline double get_point_intensity(vec2i base, vec2i point) {
        // shift intensity!
        const double intensity_min = 0.0;
        const double intensity_max = 0.8;
        double orig =  (double)
                            hash(
                                hash(
                                    hash(
                                        base.x() + hash(base.y())
                                    ) + point.x()
                                ) + point.y()
                            ) / hash_max;
        
        return orig * (intensity_max - intensity_min) + intensity_min;
    }

    /* - returns
     * 1. a point (in [0, 1]^2 when regulated)
     * 2. a seed for next iteration
     * 3. whether we're going to have the next point
     */
    inline vec2i get_point(int m, double prob_next, bool &next_iter, int &seed) {
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
    inline std::vector<vec2i> get_grid_points(int grid_x, int grid_y) {
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