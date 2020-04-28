#pragma once

#include <assert.h>
#include <algorithm>
#include <cmath>

template<typename T>
class vec2 {
private:
    T x_;
    T y_;
public:

    inline T x() {
        return this->x_;
    }

    inline T y() {
        return this->y_;
    }

    // Only check floating constant
    // Todo: check if T matches floating constant
    template <typename Floating, 
              std::enable_if_t<
                std::is_floating_point<Floating>::value, int
              > = 0
    >
    vec2(Floating x, Floating y) : x_(x), y_(y) {
        // pass
        assert(isfinite(x));
        assert(isfinite(y));
    }

    template <typename Integer,
              std::enable_if_t<
                std::negation<std::is_floating_point<Integer>>::value, int
              > = 0
    >
    vec2(Integer x, Integer y) : x_(x), y_(y) {
        // pass
    }

    vec2() : x_(0), y_(0) {
        // pass
    }

    inline vec2<T> operator+(vec2<T> &other) {
        return vec2<T>(other.x_ + x_, other.y_ + y_);
    }

    inline vec2<T> operator-(vec2<T> &other) {
        return vec2<T>(x_ - other.x_, y_ - other.y_);
    }

    inline vec2<T> operator*(T val) {
        return vec2<T>(x_ * val, y_ * val);
    }

    // Only works for float
    template <typename Floating, 
              std::enable_if_t<
                std::is_floating_point<Floating>::value, int
              > = 0
    >
    inline vec2<T> operator/(Floating val) {
        return vec2<T>(x_ / val, y_ / val);
    }

    inline T dot(vec2<T> &other) {
        return other.x_ * this->x_ + other.y_ * this->y_;
    }

    inline T squared_norm() {
        return this->x_ * this->x_ + this->y_ * this->y_;
    }

    inline vec2<T> normalize() {
        T norm = std::sqrt(x_ * x_ + y_ * y_);
        assert(isfinite(norm) && norm != 0);
        return vec2<T>(x_ / norm, y_ / norm);
    }

    template<typename U>
    vec2<U> cast_to() {
        return vec2<U>(static_cast<U>(x_), static_cast<U>(y_));
    }
};

using vec2d = vec2<double>;
using vec2i = vec2<int>;