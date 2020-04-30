#include <generators/Generators.h>

PerlinNoiseGenerator2D::PerlinNoiseGenerator2D(int grid_max) : grid_max(grid_max) {
    // generate random gradients
    rand_G = std::make_unique<vec2d[]>(grid_max * grid_max);
    for (int i = 0; i < grid_max * grid_max; i++) {
        rand_G[i] = vec2d(rand() - (RAND_MAX / 2), rand() - (RAND_MAX / 2)).normalize();
    }
}


