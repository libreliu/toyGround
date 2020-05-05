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

template <typename G>
class FractualNoiseGenerator2D {
public:
    template <typename... U>
    FractualNoiseGenerator2D(int level, int grid_max, U&&... u) {
        double weight = 1;
        double total_weight = 0;
        for (int i = 0; i < level; i++) {
            generators.push_back(std::make_unique<G>(grid_max, std::forward<U>(u)...));
            weights.push_back(weight);
            total_weight += weight;
            weight /= 2;
            grid_max /= 2;
        }

        //for (int i = 0; i < level; i++) {
        //    weights[i] /= total_weight;
        //}

    }

    double get(int i, int j) {
        double res = 0;
        for (int m = 0; m < generators.size(); m++) {
            res += generators[m].get()->get(i, j) * weights[m];
        }
        return res;
    }
private:
    std::vector<std::unique_ptr<G>> generators;
    std::vector<double> weights;
};