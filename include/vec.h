#pragma once

#include <assert.h>
#include <algorithm>
#include <cmath>
#include <type_traits>

template<typename T, std::size_t N>
class vec {
private:
    // TODO: Partial specialization for N == 0, 1, 2, 3, 4
    T M_instance[N];

    template <typename Integer,
              std::enable_if_t<
                std::negation<std::is_floating_point<Integer>>::value, int
              > = 0, typename... Args
    >
    void _vec_assign(std::size_t startPos, Integer arg, Args... Remaining) {
        M_instance[startPos] = arg;

        // The "constexpr" is necessary, or _vec_assign instantation errors will occur
        if constexpr (sizeof...(Remaining) >= 1) {
            _vec_assign(startPos + 1, Remaining...);
        }
    }

    template <typename Floating, 
              std::enable_if_t<
                std::is_floating_point<Floating>::value, int
              > = 0, typename... Args>
    void _vec_assign(std::size_t startPos, Floating arg, Args... Remaining) {
        assert(std::isfinite(arg));
        M_instance[startPos] = arg;

        // The "constexpr" is necessary, or _vec_assign instantation errors will occur
        if constexpr (sizeof...(Remaining) >= 1) {
            _vec_assign(startPos + 1, Remaining...);
        }
    }

public:
    vec() {
        for (std::size_t i = 0; i < N; i++) {
            M_instance[i] = 0;
        }
    }

    // 0. Check for Float or Int
    // 1. Check if we've got sufficient args
    // 2. Pass to _vec_initialize private function
    template <typename... Args>
    vec(Args... init) {
        static_assert(sizeof...(init) == N, "Initialization shall be N arguments in total");
        _vec_assign(0, init...);
    }

    vec(const vec<T, N> &other) {
        for (std::size_t i = 0; i < N; i++) {
            M_instance[i] = other.M_instance[i];
        }
    }

    const vec<T, N> &operator=(const vec<T, N> &other) {
        for (std::size_t i = 0; i < N; i++) {
            M_instance[i] = other.M_instance[i];
        }
        return *this;
    }

    const T& x() {
        static_assert(N >= 1, "N >= 1 required");
        return M_instance[0];
    }

    const T& y() {
        static_assert(N >= 2, "N >= 2 required");
        return M_instance[1];
    }

    const T& z() {
        static_assert(N >= 3, "N >= 3 required");
        return M_instance[2];
    }

    const T& w() {
        static_assert(N >= 4, "N >= 4 required");
        return M_instance[3];
    }

    inline T& operator[](std::size_t idx) {
        return M_instance[idx];
    }

    inline vec<T, N> operator+(const vec<T, N> &other) {
        vec<T, N> ret;
        for (std::size_t i = 0; i < N; i++) {
            ret[i] = M_instance[i] + other.M_instance[i];
        }
        return ret;
    }

    inline vec<T, N> operator-(const vec<T, N> &other) {
        vec<T, N> ret;
        for (std::size_t i = 0; i < N; i++) {
            ret[i] = M_instance[i] - other.M_instance[i];
        }
        return ret;
    }

    inline vec<T, N> operator*(T val) {
        vec<T, N> ret;
        for (std::size_t i = 0; i < N; i++) {
            ret[i] = M_instance[i] * val;
        }
        return ret;
    }

    template <typename Floating, 
              std::enable_if_t<
                std::is_floating_point<Floating>::value, int
              > = 0
    >
    inline vec<T, N> operator/(Floating val) {
        vec<T, N> ret;
        for (std::size_t i = 0; i < N; i++) {
            ret[i] = M_instance[i] / val;
        }
        return ret;
    }

    inline T dot(const vec<T, N> &other) {
        T dot_product = 0;
        for (std::size_t i = 0; i < N; i++) {
            dot_product += M_instance[i] * other.M_instance[i];
        }
        return dot_product;
    }

    inline vec<T, N> normalize() {
        T norm = std::sqrt(this->dot(*this));
        assert(std::isfinite(norm) && norm != 0);

        return vec<T, N>(*this) / norm;
    }

    template<typename U>
    vec<U, N> cast_to() {
        vec<U, N> ret;
        for (std::size_t i = 0; i < N; i++) {
            ret[i] = static_cast<U>(M_instance[i]);
        }
        return ret;
    }

    inline T squared_norm() {
        T res = 0;
        for (std::size_t i = 0; i < N; i++) {
            res += M_instance[i] * M_instance[i];
        }
        return res;
    }
};

template <typename T>
using vec2 = vec<T, 2>;
using vec2d = vec2<double>;
using vec2i = vec2<int>;

// template<typename T>
// class vec2 {
// private:
//     T x_;
//     T y_;
// public:

//     inline T x() {
//         return this->x_;
//     }

//     inline T y() {
//         return this->y_;
//     }

//     // Only check floating constant
//     // Todo: check if T matches floating constant
//     template <typename Floating, 
//               std::enable_if_t<
//                 std::is_floating_point<Floating>::value, int
//               > = 0
//     >
//     vec2(Floating x, Floating y) : x_(x), y_(y) {
//         // pass
//         assert(std::isfinite(x));
//         assert(std::isfinite(y));
//     }

//     template <typename Integer,
//               std::enable_if_t<
//                 std::negation<std::is_floating_point<Integer>>::value, int
//               > = 0
//     >
//     vec2(Integer x, Integer y) : x_(x), y_(y) {
//         // pass
//     }

//     vec2() : x_(0), y_(0) {
//         // pass
//     }

//     inline vec2<T> operator+(const vec2<T> &other) {
//         return vec2<T>(other.x_ + x_, other.y_ + y_);
//     }

//     inline vec2<T> operator-(const vec2<T> &other) {
//         return vec2<T>(x_ - other.x_, y_ - other.y_);
//     }

//     inline vec2<T> operator*(T val) {
//         return vec2<T>(x_ * val, y_ * val);
//     }

//     // Only works for float
//     template <typename Floating, 
//               std::enable_if_t<
//                 std::is_floating_point<Floating>::value, int
//               > = 0
//     >
//     inline vec2<T> operator/(Floating val) {
//         return vec2<T>(x_ / val, y_ / val);
//     }

//     inline T dot(const vec2<T> &other) {
//         return other.x_ * this->x_ + other.y_ * this->y_;
//     }

//     inline T squared_norm() {
//         return this->x_ * this->x_ + this->y_ * this->y_;
//     }

//     inline vec2<T> normalize() {
//         T norm = std::sqrt(x_ * x_ + y_ * y_);
//         assert(std::isfinite(norm) && norm != 0);
//         return vec2<T>(x_ / norm, y_ / norm);
//     }

//     template<typename U>
//     vec2<U> cast_to() {
//         return vec2<U>(static_cast<U>(x_), static_cast<U>(y_));
//     }
// };

// using vec2d = vec2<double>;
// using vec2i = vec2<int>;