#ifndef __TENSOR_HPP
#define __TENSOR_HPP

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstddef>
#include <utility>
#include <vector>
#include <cmath>
#include <limits>
#include <util.hpp>
#include <random>
#include <array>

// define BLAS_ENABLED before including this file and link against
// a blas library to speed up algebra operations
#ifdef BLAS_ENABLED
#include <cblas.h>
#endif

/* Numpy like multi dimensional array that supports most of the functionality
 * of its python counterpart but uses templates instead.
 * Because of the template this array has the same performace properties
 * like custom implementations and additional features such as printing
 * to std::cout and using numpy like savetxt and loadtxt to process csv files
 * There is also support for basic linalg operations such as array additions
 * and matrix multiplication but none of them is optimized so beware of
 * performance bottlenecks.
 *
 * I basically implemented this to access elements in an array(a, b, c) fashion
 * without passing pointers or something but having the compiler treat the
 * templates like variables. Pretty cool and it does not use recursion.
 *
 * The ndarray can store any datatype but be aware that not all types allow
 * for all operation, such as algebraic addition on strings may result in strange
 * results.
 *
 * Furthermore the code makes heavy use of exceptions for error handling so if
 * you are unfamiliar with them or do not use them in your code, you better do so
 * as they make life a loooooot easier.
 */
template <size_t N, typename T>
class Tensor {
public:
    static_assert(N >= 1, "number of dimensions has to be at least 1");

    typedef T*        iterator;
    typedef const T*  const_iterator;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;

    // reverse iterators
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    template <typename ...Args>
    static Tensor ones(Args ...args) {
        static_assert(std::is_fundamental<T>::value || std::is_arithmetic<T>::value,
                "cannot numerically initialize non fundamental type");
      Tensor array(args...);

      for (auto &e : array) {
          e = (T) 1;
      }
      return array;
    }

    template <typename ...Args>
    static Tensor zeros(Args ...args) {
        static_assert(std::is_fundamental<T>::value || std::is_arithmetic<T>::value,
                      "cannot numerically initialize non fundamental type");
        Tensor array(args...);

      for (auto &e : array) {
          e = (T) 0;
      }
      return array;
    }

    template <typename ...Args>
    static Tensor random(Args ...args) {
        static_assert(std::is_fundamental<T>::value || std::is_arithmetic<T>::value,
                      "cannot numerically initialize non fundamental type");
        Tensor array(args...);

      std::random_device rd;
      std::default_random_engine engine(rd());
      if (util::is_floating_point_type<T>()) {
          std::uniform_real_distribution<T> dist(0.0, 1.0);
          for (auto &e : array) {
              e = (T) dist(engine);
          }
      }

      return array;
    }

    // default constructor
    Tensor() = default;

    /* Template contructor. Called with N integer values. Creates an Tensor of shape
     * args[0] x args[1] x ... args[N - 1] that elements can be accessed via the overloaded
     * operator(Args ...args). Elements are stored continuously in memory leading to the
     * most efficient code possible.
     */
    template <typename ...Args>
    explicit Tensor(Args...args) {
      static_assert(sizeof...(args) == N, "invalid number of arguments");

      const size_t d[N] = { size_t(args)... };
      for (size_t i = 0; i < N; ++i) {
        _shape[i] = d[i];
      }
      get_size();
      if (_size == 0) {
          throw std::runtime_error("array size cannot be 0");
      }
      _data = new T[size()];
    }

    /* Create Tensor of shape shape[0] x shape[1] x ... shape[N - 1].
     * If the provided vector has to many elements an exception is thrown.
     */
    template <typename Int>
    explicit Tensor(const std::vector<Int> &shape) {
      static_assert(std::is_integral<Int>::value, "shape type must be integer");

      if (shape.size() != N) {
        throw std::runtime_error("invalid number of arguments");
      }

      for (size_t i = 0; i < N; ++i) {
        _shape[i] = shape[i] > 0 ? (size_t) shape[i] : throw std::runtime_error("invalid shape");
      }
      get_size();
      if (_size == 0) {
          throw std::runtime_error("array size cannot be 0");
      }
      _data = new T[size()];
    }

    // copy constructor
    Tensor(const Tensor &array) {
      for (size_t i = 0; i < N; ++i) {
        _shape[i] = array._shape[i];
      }
      get_size();
      _data = new T[size()];

      if (std::is_fundamental<T>::value) {
        memcpy(_data, array._data, size() * sizeof(T));
      } else {
        auto it = begin();
        for (const auto &e : array)
          *it++ = e;
      }
    }

    // move constructor
    Tensor(Tensor &&array) noexcept {
      swap(array);
    }

    // destructor
    ~Tensor() {
      clear();
    }

    // copy assingment operator
    Tensor& operator=(const Tensor &array) {
      if (!empty())
        clear();
      _data = new T[array.size()];

      for (size_t i = 0; i < N; ++i) {
        _shape[i] = array._shape[i];
      }
      get_size();

      // check im memcpy everything is save
      if (std::is_fundamental<T>::value) {
        memcpy(_data, array._data, size() * sizeof(T));
      } else {
        for (size_t i = 0; i < size(); ++i) {
          (*this)[i] = array[i];
        }
      }
      return *this;
    }

    // move assignment operatpr
    Tensor& operator=(Tensor &&array) noexcept {
      swap(array);
      return *this;
    }

    /* Array index operator. Access the elements natively by providing as many
     * indices as necessary.
     */
    template <typename ...Args>
    reference operator()(Args ...args) {
      static_assert(sizeof...(args) == N, "invalid number of arguments");
      return _data[get_index(std::forward<Args>(args)...)];
    }

    template <typename ...Args>
    const_reference operator()(Args...args) const {
      static_assert(sizeof...(args) == N, "invalid number of arguments");
      return _data[get_index(std::forward<Args>(args)...)];
    }

    reference operator[](size_t i) {
        if (N == 1) {
            return (*this)(i);
        } else {
            throw std::runtime_error("invalid function call");
        }
    }

    const_reference operator[](size_t i) const {
        if (N == 1) {
            return (*this)(i);
        } else {
            throw std::runtime_error("invalid function call");
        }
    }

    template <typename ...Args>
    reference at(Args ...args) {
      return at(std::forward(args)...);
    }

    template <typename ...Args>
    const_reference at(Args ...args) const {
      static_assert(sizeof...(args) == N, "invalid number of arguments");
      const size_t idx[N] = { size_t(args)... };

      for (size_t i = 0; i < N; ++i) {
        if (idx[i] > _shape[i]) {
          throw std::range_error("index out of range");
        }
      }
      return (*this)(std::forward(args)...);
    }

    // swap arrays
    void swap(Tensor &array) {
      std::swap(_data, array._data);
      std::swap(_size, array._size);
      for (size_t i = 0; i < N; ++i)
        std::swap(_shape[i], array._shape[i]);
    }

    // get shape as vector
    std::array<unsigned, N> shape() const {
      std::array<unsigned, N> shape(_shape, _shape + N);
      return shape;
    }

    // get the number of dimensions along one axis (aka the tensors dimension)
    size_t shape(size_t dim) const {
      return _shape[dim];
    }

    size_t dimensions() const {
      return N;
    }

    // number of stored elements
    size_t size() const {
      return _size;
    }

    // check if empty
    bool empty() const {
      return _size == 0;
    }

    // free memory and set shapes and size to zero
    void clear() {
        if (!_data) {
            delete[] _data;
            _data = nullptr;
        }

      for (size_t i = 0; i < N; ++i) {
        _shape[i] = 0;
      }
      _size = 0;
    }

    // check for equality
    bool operator==(const Tensor &array) const {
      if (size() != array.size()) {
        return false;
      } else if (this == &array) {
        return true;
      }

      for (size_t i = 0; i < size(); ++i) {
        if (_data[i] != array._data[i])
          return false;
      }
      return true;
    }

    // check for  inequallity
    bool operator!=(const Tensor &array) const {
      return !(*this == array);
    }

    // iterator pointing to the first elements
    iterator begin() {
      return _data;
    }

    // iterator pointing to behind the last elements
    iterator end() {
      return _data + size();
    }

    const_iterator begin() const {
      return _data;
    }

    const_iterator end() const {
      return _data + size();
    }

    reverse_iterator rbegin() {
      return reverse_iterator(end());
    }

    reverse_iterator rend() {
      return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const {
      return reverse_iterator(end());
    }

    const_reverse_iterator rend() const {
      return reverse_iterator(begin());
    }

    pointer data() {
      return _data;
    }

    const_pointer data() const {
      return _data;
    }

    // elementwise addition
    Tensor operator+(const Tensor &array) const {
      if (size() != array.size())
        throw std::runtime_error("shapes mismatch");

      Tensor temp = *this;
      temp += array;
      return temp;
    }

    Tensor& operator+=(const Tensor &array) {
      if (size() != array.size())
        throw std::runtime_error("shapes mismatch");

      for (size_t i = 0; i < size(); ++i) {
        _data[i] += array._data[i];
      }

      return *this;
    }

    // elementwise subtraction
    Tensor operator-(const Tensor &array) const {
      if (size() != array.size())
        throw std::runtime_error("shapes mismatch");

      Tensor temp = *this;
      temp -= array;
      return temp;
    }

    Tensor& operator-=(const Tensor &array) {
      if (size() != array.size())
        throw std::runtime_error("shapes mismatch");

      for (size_t i = 0; i < size(); ++i) {
        _data[i] -= array._data[i];
      }

      return *this;
    }

    // elementwise multiplication
    Tensor operator*(const Tensor &array) const {
      if (size() != array.size())
        throw std::runtime_error("shapes mismatch");

      Tensor temp = *this;
      temp *= array;
      return temp;
    }

    Tensor& operator*=(const Tensor &array) {
      if (size() != array.size())
        throw std::runtime_error("shapes mismatch");

      for (size_t i = 0; i < size(); ++i) {
        _data[i] *= array._data[i];
      }

      return *this;
    }

    // elementwise division
    Tensor operator/(const Tensor &array) const {
      if (size() != array.size())
        throw std::runtime_error("shapes mismatch");

      Tensor temp = *this;
      temp /= array;
      return temp;
    }

    Tensor& operator/=(const Tensor &array) {
      if (size() != array.size())
        throw std::runtime_error("shapes mismatch");

      for (size_t i = 0; i < size(); ++i) {
        _data[i] /= array._data[i];
      }

      return *this;
    }

    // scalar multiplication
    Tensor operator*(const T &k) const {
      Tensor temp = *this;
      temp *= k;
      return temp;
    }

    Tensor& operator*=(const T &k) {
      for (size_t i = 0; i < size(); ++i) {
        _data[i] *= k;
      }

      return *this;
    }

    // scalar division
    Tensor operator/(const T &k) const {
      Tensor temp = *this;
      temp /= k;
      return temp;
    }

    Tensor& operator/=(const T &k) {
      for (size_t i = 0; i < size(); ++i) {
        _data[i] /= k;
      }

      return *this;
    }

    // assign elements to arrays
    template <typename It>
    void assign(const It &begin, const It &end) {
      if ((size_t) std::distance(begin, end) != size())
        throw std::runtime_error("shapes mismatch");

      auto ptr = _data;
      for (auto it = begin; it != end; ++it) {
        *ptr++ = *it;
      }
    }

    // change shape
    template <typename ...Args>
    void reshape(Args ...args) {
      static_assert(sizeof...(args) == N, "invalid number of arguments");

      const size_t shape[N] = { size_t(args)... };
      size_t s = 1;
      for (size_t i = 0; i < N; ++i)
        s *= shape[i];

      if (s == _size) {
        for (size_t i = 0; i < N; ++i) {
          _shape[i] = shape[i];
        }
      } else {
        throw std::runtime_error("invalid shape");
      }
    }

    /* Numpy like savetxt function to store 1D and 2D arrays in a file.
     * TODO: write some more...
     */
    void savetxt(const std::string &fname, const std::string &delim=", ", char newline='\n', char comments='#') const {
      if (N > 2) {
        throw std::runtime_error("savetxt only works with 1D or 2D arrays");
      }

      std::ofstream file(fname);
      if (!file) {
        throw std::runtime_error("could not open file");
      }

      if (N == 1) {
        size_t i = 0;
        for (const auto &e : *this) {
          file << e;
          if ((i + 1) < size())
            file << delim;
          i += 1;
        }
      } else {
        size_t i = 0, j = 0;
        for (const auto &e : *this) {
          file << e;
          if ((i + 1) < shape(0)) {
            file << delim;
            i += 1;
          } else {
            if ((j + 1) < shape(1))
              file << newline;
            i = 0;
            j += 1;
          }
        }
      }

      file.close();
    }

    /* Numpy like loadtxt function to read array from file
     * TODO: write some more...
     */
    void loadtxt(const std::string &fname, const std::string &delim=", ", char newline='\n', char comments='#') {
      if (N > 2) {
        throw std::runtime_error("loadtxt only works with 1D or 2D arrays");
      }

      std::ifstream file(fname);
      if (!file) {
        throw std::runtime_error("could not open file");
      }

      if (N == 1) {
        std::string line;
        std::getline(file, line, newline);
        std::vector<std::string> tokens = util::split(line, delim);
        *this = Tensor<1, T>(tokens.size());
        auto it = begin();
        for (size_t i = 0; i < size(); ++i) {
          *it++ = util::strto<T>(tokens[i]);
        }
      } else {
        std::string line;
        std::vector<std::vector<std::string>> tokens;
        // read and split all lines of the file that
        // do not start with comments character
        while (!file.eof()) {
          std::getline(file, line, newline);
          if (line[0] != comments)
            tokens.emplace_back(util::split(line, delim));
        }

        // get the maximum number of cols
        size_t max_cols = 0;
        for (size_t i = 0; i < tokens.size(); ++i)
          max_cols = tokens[i].size() > max_cols ? tokens[i].size() : max_cols;

        // create matrix
        *this = Tensor<2, T>(tokens.size(), max_cols);

        // convert strings to numbers
        auto it = begin();
        for (size_t i = 0; i < tokens.size(); ++i) {
          size_t j;
          for (j = 0; j < tokens[i].size(); ++j) {
            *it++ = util::strto<T>(tokens[i][j]);
          }
          // fill lines with less than m cols with the default value 0
          for (; j < max_cols; ++j) {
            *it++ = 0;
          }
        }
      }

      file.close();
    }

    friend std::ofstream& operator<<(std::ofstream &of, const Tensor &array) {
        of.write((const char *) N, sizeof(N));
        of.write((const char *) array._shape, N * sizeof(size_t));
        of.write((const char *) array.data(), array.size() * sizeof(T));
        return of;
    }

    friend std::ifstream& operator>>(std::ifstream &inf, Tensor &array) {
        array.clear();
        size_t dim;
        inf.read((char *) &dim, sizeof(dim));
        if (dim != N) {
            throw std::runtime_error("invalid dimension, should be" + std::to_string(N) + " is " + std::to_string(dim));
        }
        inf.read((char *) array._shape, N * sizeof(size_t));
        array._size = array.get_size();
        array._data = new T[array._size];
        inf.read((char *) array.data(), array.size() * sizeof(T));
        return inf;
    }

private:
    // dummmy constructor, because templates have downsides
    template <size_t M>
    explicit Tensor(const Tensor<M, T> &array) {
      throw std::runtime_error("called dummy constructor");
    }

    // compute n-d index in row major storage memory layout
    // specializations of the computation are provided up to
    // 4D, any optimizing compiler will remove the other options
    // leading to code that is not slower (and sometimes faster)
    // than doing naive index computations on arrays.
    template <typename ...Args>
    size_t get_index(Args ...args) const {

      const size_t idx[N] = { size_t(args)... };

      switch (N) {
        case 1: {
          return idx[0];
        } case 2: {
          return idx[0] * _shape[1] + idx[1];
        } case 3: {
          return (idx[0] * _shape[1] + idx[1]) * _shape[2] + idx[2];
        } case 4: {
          return ((idx[0] * _shape[1] + idx[1]) * _shape[2] + idx[2]) * _shape[3] + idx[3];
        } default: {
          // handles cases of 5D tensors and higher
          size_t index = idx[0];
          for (size_t i = 1; i < N; ++i) {
            index = (index * _shape[i]) + idx[i];
          }
          return index;
        }
      }
    }

    // compute size
    void get_size() {
      _size = 1;
      for (size_t i = 0; i < N; ++i) {
        _size *= _shape[i];
      }
    }

    // pointer to the begin of storage
    T *_data = nullptr;

    // tensor shape
    size_t _shape[N] = { 0 };

    // number of elements
    size_t _size = 0;

};

template <size_t N, typename T>
using tensor = Tensor<N, T>;

template <typename T>
using vector = Tensor<1, T>;

template <typename T>
using matrix = Tensor<2, T>;

template <typename T>
using cube = Tensor<3, T>;

template <typename T>
inline T dot(const vector<T> &X, const vector<T> &Y) {
  if (X.size() != Y.size())
    throw std::runtime_error("shapes mismatch");

  T res = 0;
  for (size_t i = 0; i < X.size(); ++i) {
    res += X(i) * Y(i);
  }
  return res;
}

#ifdef BLAS_ENABLED
template <>
inline float dot(const vector<float> &X, const vector<float> &Y) {
  if (X.size() != Y.size())
    throw std::runtime_error("shapes mismatch");

  return cblas_sdot(X.size(), X.data(), 1, Y.data(), 1);
}

template <>
inline double dot(const vector<double> &X, const vector<double> &Y) {
  if (X.size() != Y.size())
    throw std::runtime_error("shapes mismatch");

  return cblas_ddot(X.size(), X.data(), 1, Y.data(), 1);
}
#endif

template <typename T>
inline vector<T> dot(const matrix<T> &A, const vector<T> &X) {
  if (A.shape(1) != X.size())
    throw std::runtime_error(std::string("shapes mismatch ") + std::to_string(A.shape(1)) + " != " + std::to_string(X.size()));

  vector<T> Y(A.shape(0));
  for (size_t i = 0; i < A.shape(0); ++i) {
    T accum = 0;
    for (size_t j = 0; j < A.shape(1); ++j) {
      accum += A(i, j) * X(j);
    }
    Y(i) = accum;
  }
  return Y;
}

#ifdef BLAS_ENABLED
template <>
inline vector<float> dot(const matrix<float> &A, const vector<float> &X) {
  if (A.shape(1) != X.size())
    throw std::runtime_error("shapes mismatch");

  vector<float> Y(A.shape(0));
  cblas_sgemv(CblasRowMajor, CblasNoTrans, (int) A.shape(0), (int) A.shape(1), 1.0f, A.data(), (int) A.shape(0), X.data(), 1, 0, Y.data(), 1);
  return Y;
}

template <>
inline vector<double> dot(const matrix<double> &A, const vector<double> &X) {
  if (A.shape(1) != X.size())
    throw std::runtime_error("shapes mismatch");

  vector<double> Y(A.shape(0));
  cblas_dgemv(CblasRowMajor, CblasNoTrans, (int) A.shape(0), (int) A.shape(1), 1.0, A.data(), (int) A.shape(0), X.data(), 1, 0, Y.data(), 1);
  return Y;
}
#endif

template <typename T>
inline vector<T> dot(const vector<T> &X, const matrix<T> &A) {
  if (A.shape(0) != X.size())
    throw std::runtime_error("shapes mismatch");

  vector<T> Y(A.shape(1));
  for (size_t j = 0; j < A.shape(1); ++j) {
    T accum = 0;
    for (size_t i = 0; i < A.shape(0); ++i) {
      accum += A(i, j) * X(i);
    }
    Y(j) = accum;
  }
  return Y;
}

template <typename T>
inline matrix<T> dot(const matrix<T> &A, const matrix<T> &B) {
  if (A.shape(1) != B.shape(0))
    throw std::runtime_error("shapes mismatch");

  const size_t n = A.shape(0), m = B.shape(1), r = A.shape(1);
  matrix<T> C(n, m);

  for (size_t i = 0; i < n; ++i) {
    for (size_t k = 0; k < r; ++k) {
      for (size_t j = 0; j < m; ++j) {
        C(i, j) = A(i, k) * B(k, j);
      }
    }
  }
  return C;
}

#ifdef BLAS_ENABLED
template <>
inline matrix<float> dot(const matrix<float> &A, const matrix<float> &B) {
  if (A.shape(1) != B.shape(0))
    throw std::runtime_error("shapes mismatch");

  const size_t n = A.shape(0), m = B.shape(1), k = A.shape(1);
  matrix<float> C(n, m);
  cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasTrans, n, m, k, 1.0f, A.data(), (int) A.shape(0), B.data(), B.shape(0), 0, C.data(), C.shape(0));
  return C;
}

template <>
inline matrix<double> dot(const matrix<double> &A, const matrix<double> &B) {
  if (A.shape(1) != B.shape(0))
    throw std::runtime_error("shapes mismatch");

  const int n = A.shape(0), m = B.shape(1), k = A.shape(1);
  matrix<double> C(n, m);
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, n, m, k, 1.0f, A.data(), (int) A.shape(0), B.data(), (int) B.shape(0), 0, C.data(), C.shape(0));
  return C;
}
#endif

template <typename T>
inline matrix<T> matrix_power(const matrix<T> &mat, size_t exp) {
  if (mat.shape(0) != mat.shape(1))
    throw std::runtime_error("matrix is not quadratic");

  if (exp == 1) {
    return matrix<T>(mat);
  } else if (exp % 2 == 0){
    return power(dot(mat, mat), exp / 2);
  } else {
    return dot(power(dot(mat, mat), (exp - 1) / 2), mat);
  }
}

template <typename T>
matrix<T> transpose(const matrix<T> &mat) {
  matrix<T> tmp(mat.shape(1), mat.shape(0));
  for (size_t i = 0; i < mat.shape(0); ++i) {
    for (size_t j = 0; j < mat.shape(1); ++j) {
      tmp(j, i) = mat(i, j);
    }
  }
  return tmp;
}

template <size_t N, typename T>
inline double norm(const Tensor<N, T> &array) {
  double res = 0.0;
  for (const auto &e : array) {
    res += e * e;
  }
  return std::sqrt(res);
}

template <typename T>
inline std::ostream& operator<<(std::ostream &os, const vector<T> &vector) {
  os << '[';
  for (size_t i = 0; i < vector.size(); ++i) {
    os << vector(i);
    if ((i + 1) != vector.size())
      os << ", ";
  }
  return os << ']';
}

template <typename T>
inline std::ostream& operator<<(std::ostream &os, const matrix<T> &matrix) {
  os << '[';
  for (size_t i = 0; i < matrix.shape(0); ++i) {
    if (i > 0)
      os << ' ';
    os << '[';
    for (size_t j = 0; j < matrix.shape(1); ++j) {
      os << matrix(i, j);
      if ((j + 1) != matrix.shape(1))
        os << ", ";
    }
    os << ']';
    if ((i + 1) != matrix.shape(0))
      os << std::endl;
  }
  return os << ']';
}

#endif // __TENSOR_HPP
