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
    PerlinNoiseGenerator2D(int grid_max);

    inline vec2d get_pseudo_grad(int m, int n) {
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

    inline double get(int x_in, int y_in) {
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