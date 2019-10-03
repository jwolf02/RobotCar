#ifndef __COMMON_HPP
#define __COMMON_HPP

#include <vector>
#include <cmath>
#include <functional>
#include <random>
#include <cassert>
#include <string>
#include <type_traits>
#include <stdexcept>

// switch for using optimized vector operations
#define OPTIMIZATION

// Compute dot product between two vectors
inline double dot(const std::vector<double> &X, const std::vector<double> &Y) {
  assert(X.size() == Y.size());

  double res = 0.0;
  unsigned i;
  #ifdef OPTIMIZATION
  for (i = 0; i < X.size(); i += 4) {
    double a, b, c, d;
    a = X[i    ] * Y[i    ];
    b = X[i + 1] * Y[i + 1];
    c = X[i + 2] * Y[i + 2];
    d = X[i + 3] * Y[i + 3];
    res += (a + b) + (c + d);
  }
  #else
  i = 0;
  #endif
  for (; i < X.size(); ++i)
    res += X[i] * Y[i];

  return res;
}

// sigmoid function
template <typename value_t>
inline value_t sigmoid(value_t x) {
  return 1.0 / (1 + std::exp(-x));
}

// elementwise sigmoid function on vector
inline std::vector<double> sigmoid(const std::vector<double> &X) {
  std::vector<double> Y(X.size());
  for (unsigned i = 0; i < X.size(); ++i)
    Y[i] = sigmoid(X[i]);
  return Y;
}

// compute soft max to create probability distribution
inline std::vector<double> softmax(const std::vector<double> &X) {
  std::vector<double> Y(X.size());

  // compute exponential over all vector elements
  double sum = 0.0;
  for (unsigned i = 0; i < X.size(); ++i) {
    sum += (Y[i] = std::exp(X[i]));
  }

  for (auto &v : Y) {
    v /= sum;
  }

  return Y;
}

template <typename T, typename func_t>
inline T reduce(const std::vector<T> &vector, func_t f) {
  assert(!vector.empty());

  T tmp = vector[0];
  for (size_t i = 0; i < vector.size(); ++i)
    tmp = f(tmp, vector[i]);
  return tmp;
}

template <typename T>
inline unsigned argmax(const std::vector<T> &X) {
  assert(!X.empty());

  T max = X[0];
  unsigned idx = 0;
  for (unsigned i = 1; i < X.size(); ++i) {
    if (max < X[i]) {
      max = X[i];
      idx = i;
    }
  }
  return idx;
}

template <typename T>
inline unsigned argmin(const std::vector<T> &X) {
  assert(!X.empty());

  T min = X[0];
  unsigned idx = 0;
  for (unsigned i = 1; i < X.size(); ++i) {
    if (X[i] < min) {
      min = X[i];
      idx = i;
    }
  }
  return idx;
}

template <typename T>
inline T max(const std::vector<T> &X) {
  assert(!X.empty());

  T max = X[0];
  for (unsigned i = 0; i < X.size(); ++i) {
    max = max < X[i] ? X[i] : max;
  }
  return max;
}

template <typename T>
inline T min(const std::vector<T> &X) {
  assert(!X.empty());

  T min = X[0];
  for (unsigned i = 0; i < X.size(); ++i) {
    min = X[i] < min ? X[i] : min;
  }
  return min;
}

template <typename T>
inline void shuffle(std::vector<T> &vector) {
  std::random_device rd;
  std::default_random_engine engine(rd());
  std::uniform_int_distribution<unsigned> dist(0, (unsigned) vector.size() - 1);

  for (unsigned i = 0; i < vector.size(); ++i) {
    unsigned r = dist(engine);
    T tmp = vector[r];
    vector[r] = std::move(vector[i]);
    vector[i] = std::move(tmp);
  }
}

template <class T>
std::vector<T>& operator+=(std::vector<T>& lhs, const std::vector<T>& rhs) {
  assert(lhs.size() == rhs.size());
  for (uint i=0; i<rhs.size(); i++)
    lhs[i] += rhs[i];
  return lhs;
}

template <class T>
const std::vector<T> operator+(const std::vector<T>& lhs, const std::vector<T>& rhs) {
  std::vector<T> result = lhs;
  result += rhs;
  return result;
}

template <class T>
std::vector<T>& operator-=(std::vector<T>& lhs, const std::vector<T>& rhs) {
  assert(lhs.size() == rhs.size());
  for (uint i=0; i<rhs.size(); i++)
    lhs[i] -= rhs[i];
  return lhs;
}

template <class T>
const std::vector<T> operator-(const std::vector<T>& lhs, const std::vector<T>& rhs) {
  std::vector<T> result = lhs;
  result -= rhs;
  return result;
}

template <class T>
std::vector<T>& operator*=(std::vector<T>& lhs, const std::vector<T>& rhs) {
  assert(lhs.size() == rhs.size());
  for (uint i=0; i<rhs.size(); i++)
    lhs[i] *= rhs[i];
  return lhs;
}

template <class T>
const std::vector<T> operator*(const std::vector<T>& lhs, const std::vector<T>& rhs) {
  std::vector<T> result = lhs;
  result *= rhs;
  return result;
}

template <class T>
std::vector<T>& operator*=(std::vector<T>& lhs, const T& rhs) {
  for (uint i=0; i<lhs.size(); i++)
    lhs[i] *= rhs;
  return lhs;
}

template <class T>
const std::vector<T> operator*(const std::vector<T>& lhs, const T& rhs) {
  std::vector<T> result = lhs;
  result *= rhs;
  return result;
}

template <class T>
const std::vector<T> operator*(const T& lhs, const std::vector<T>& rhs) {
  return rhs * lhs;
}

#endif // __COMMON_HPP
