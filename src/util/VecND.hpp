#ifndef __VECND_HPP
#define __VECND_HPP

#include <cstddef>
#include <cmath>
#include <iostream>
#include <initializer_list>

template <size_t N, typename value_t>
class VecND {
public:

    typedef value_t*        iterator;
    typedef const value_t*  const_iterator;
    typedef value_t&        reference;
    typedef const value_t&  const_reference;

    static_assert(std::is_fundamental<value_t>::value && std::is_arithmetic<value_t>::value,
            "wrapped type must be fundamental and arithmetic");
    static_assert(N > 0, "VecND dimension must not be >=1");

    VecND() = default;

    ~VecND() = default;

    VecND(const VecND &vec) {
        for (size_t i = 0; i < N; ++i) {
            _v[i] = vec._v[i];
        }
    }

    template <size_t M, typename assign_t>
    explicit VecND(const VecND<M, assign_t> &vec) {
        *this = vec;
    }

    template <typename ...Args>
    explicit VecND(Args ...args) {
        static_assert(sizeof...(args) == N, "invalid number of arguments");

        const value_t tmp[N] = { value_t(args)... };
        for (size_t i = 0; i < N; ++i) {
            _v[i] = tmp[i];
        }
    }

    VecND(const std::initializer_list<value_t> &list) {
        *this = list;
    }

    template <size_t M, typename assign_t>
    VecND& operator=(const VecND<M, assign_t> &vec) {
        for (size_t i = 0; i < std::min(N, M); ++i) {
            _v[i] = vec._v[i];
        }
        for (size_t i = std::min(N, M); i < N; ++i) {
            _v[i] = 0;
        }
    }

    VecND& operator=(const std::initializer_list<value_t> &list) {
        size_t i = 0;
        for (const auto &v : list) {
            _v[i++] = v;
        }
    }

    reference operator[](size_t i) {
        return _v[i];
    }

    const_reference operator[](size_t i) const {
        return _v[i];
    }

    bool operator==(const VecND &vec) const {
        size_t i = 0;
        while (i < N && _v[i] == vec[i])
            ++i;
        return i == N;
    }

    bool operator!=(const VecND &vec) const {
        return !(*this == vec);
    }

    VecND operator+(const VecND &vec) const {
        VecND tmp;
        for (size_t i = 0; i < N; ++i) {
            tmp[i] = _v[i] + vec[i];
        }
        return tmp;
    }

    VecND& operator+=(const VecND &vec) {
        for (size_t i = 0; i < N; ++i) {
            _v[i] += vec[i];
        }
        return *this;
    }

    VecND operator-(const VecND &vec) const {
        VecND tmp;
        for (size_t i = 0; i < N; ++i) {
            tmp[i] = _v[i] - vec[i];
        }
        return tmp;
    }

    VecND& operator-=(const VecND &vec) {
        for (size_t i = 0; i < N; ++i) {
            _v[i] -= vec[i];
        }
        return *this;
    }

    value_t operator*(const VecND &vec) const {
        value_t accum = 0;
        for (size_t i = 0; i < N; ++i) {
            accum += _v[i] * vec[i];
        }
        return accum;
    }

    VecND operator*(value_t k) const {
        VecND tmp;
        for (size_t i = 0; i < N; ++i) {
            tmp[i] = _v[i] * k;
        }
        return tmp;
    }

    VecND& operator*=(value_t k) const {
        for (size_t i = 0; i < N; ++i) {
            _v[i] *= k;
        }
        return *this;
    }

    VecND operator/(value_t k) const {
        VecND tmp;
        for (size_t i = 0; i < N; ++i) {
            tmp[i] = _v[i] / k;
        }
        return tmp;
    }

    VecND& operator/=(value_t k) const {
        for (size_t i = 0; i < N; ++i) {
            _v[i] /= k;
        }
        return *this;
    }

    iterator begin() {
        return _v;
    }

    const_iterator begin() const {
        return _v;
    }

    iterator end() {
        return _v + N;
    }

    const_iterator end() const {
        return _v + N;
    }

    value_t abs() const {
        return std::sqrt((*this) * (*this));
    }

    VecND norm() const {
        return (*this) / abs();
    }

    void normalize() {
        *this = norm();
    }

    size_t dimension() const {
        return N;
    }

    value_t distance(const VecND &vec) const {
        return (*this - vec).abs();
    }

    friend std::ostream& operator<<(std::ostream &os, const VecND &vec) {
        os << '(';
        for (size_t i = 0; i < N; ++i) {
            os << vec[i];
            if ((i + 1) != N) {
                os << ',' << ' ';
            }
        }
        os << ')';
        return os;
    }

private:

    value_t _v[N] = { 0 };

};

typedef VecND<3, int>       Int3D;
typedef VecND<3, long>      Long3D;
typedef VecND<3, float>     Float3D;
typedef VecND<3, double>    Double3D;

#endif // __VECND_HPP
