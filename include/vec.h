#pragma once

#include <assert.h>
#include <algorithm>
#include <cmath>

template<typename T>
class vec2 {
public:
    T x;
    T y;

    vec2(T x_, T y_) : x(x_), y(y_) {
        // pass
        assert(isfinite(x));
        assert(isfinite(y));
    }

    vec2() : x(0), y(0) {
        // pass
    }

    inline vec2<T> operator+(vec2<T> &other) {
        return vec2<T>(other.x + x, other.y + y);
    }

    inline vec2<T> operator-(vec2<T> &other) {
        return vec2<T>(x - other.x, y - other.y);
    }

    inline T dot(vec2<T> &other) {
        return other.x * this->x + other.y * this->y;
    }


    inline vec2<T> normalize() {
        T norm = std::sqrt(x * x + y * y);
        assert(isfinite(norm) && norm != 0);
        return vec2<T>(x / norm, y / norm);
    }
};

using vec2d = vec2<double>;