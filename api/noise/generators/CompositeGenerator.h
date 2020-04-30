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
class CompositeGenerator {
public:
    template <typename ...U>
    CompositeGenerator(int total, U&&... u) {
        for (int i = 0; i < total; i++) {
            generators.push_back(std::make_unique<T>(std::forward<U>(u)...));
        }
    }

    double get(int i, int j) {
        double res = 0;
        for (int m = 0; m < generators.size(); m++) {
            res += generators[m].get()->get(i, j);
        }
        return res;
    }
private:
    std::vector<std::unique_ptr<T>> generators;
};
