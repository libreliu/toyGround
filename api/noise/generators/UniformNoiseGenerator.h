#pragma once

#include <random>
#include <functional>

class UniformNoiseGenerator {
private:
    std::uniform_real_distribution<double> rnd_distr;
    std::default_random_engine rnd_engine;

public:
    UniformNoiseGenerator() {
        rnd_distr = std::uniform_real_distribution<double>(0.0, 1.0);
    }

    inline double get(int x, int y) {
        return rnd_distr(rnd_engine);
    }
};