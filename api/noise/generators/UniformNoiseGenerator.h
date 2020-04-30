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

class UniformNoiseGenerator {
public:
    inline double get(int x, int y) {
        return (double)rand() / RAND_MAX;
    }
};