#include <generators/Generators.h>

PerlinNoiseGenerator2D::PerlinNoiseGenerator2D(int grid_max, int seed) : grid_max(grid_max) {
    // initialize rnd_engine with seed
    rnd_distr = std::uniform_real_distribution<double>(-1.0, 1.0);
    rnd_engine = std::minstd_rand(seed);

    // generate random gradients - but they shall be same among threads
    rand_G = std::make_unique<vec2d[]>(grid_max * grid_max);
    for (int i = 0; i < grid_max * grid_max; i++) {
        rand_G[i] = vec2d(rnd_distr(rnd_engine), rnd_distr(rnd_engine)).normalize();
    }
}


