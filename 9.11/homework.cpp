
#include <iostream>
#include <utility>
#include <stdexcept>
#include <cstddef>
#include <limits>


struct Mtx {
    size_t rows = 0;
    size_t cols = 0;
    int* data = nullptr;

    Mtx() = default;

    Mtx(size_t n, size_t m) : rows(n), cols(m) {
        if (m != 0 &&
            n > std::numeric_limits<size_t>::max() / m) {
            throw std::length_error("Matrix size overflow");
        }

        if (n != 0 && m != 0) {
            data = new int[n * m]{};
        }
    }

    ~Mtx() {
        delete[] data;
    }

    Mtx(const Mtx& other)
        : Mtx(other.rows, other.cols) {
        for (size_t i = 0; i < rows * cols; ++i) {
            data[i] = other.data[i];
        }
    }

    Mtx& operator=(const Mtx& other) {
        if (this != &other) {
            Mtx temp(other);

            std::swap(rows, temp.rows);
            std::swap(cols, temp.cols);
            std::swap(data, temp.data);
        }

        return *this;
    }

    Mtx(Mtx&& other) noexcept
        : rows(other.rows),
          cols(other.cols),
          data(other.data) {
        other.rows = 0;
        other.cols = 0;
        other.data = nullptr;
    }

    Mtx& operator=(Mtx&& other) noexcept {
        if (this != &other) {
            delete[] data;

            rows = other.rows;
            cols = other.cols;
            data = other.data;

            other.rows = 0;
            other.cols = 0;
            other.data = nullptr;
        }

        return *this;
    }

    int& at(size_t i, size_t j) {
        if (i >= rows || j >= cols) {
            throw std::out_of_range("Invalid matrix index");
        }

        return data[i * cols + j];
    }

    const int& at(size_t i, size_t j) const {
        if (i >= rows || j >= cols) {
            throw std::out_of_range("Invalid matrix index");
        }

        return data[i * cols + j];
    }

    int* operator[](size_t row) {
        if (row >= rows || cols == 0) {
            throw std::out_of_range("Invalid row");
        }

        return data + row * cols;
    }

    const int* operator[](size_t row) const {
        if (row >= rows || cols == 0) {
            throw std::out_of_range("Invalid row");
        }

        return data + row * cols;
    }
};


struct Mtxs {
    size_t k = 0;
    Mtx* mtxs = nullptr;

    Mtxs() = default;

    explicit Mtxs(size_t count)
        : k(count),
          mtxs(count ? new Mtx[count] : nullptr) {}

    ~Mtxs() {
        delete[] mtxs;
    }

    Mtxs(const Mtxs& other)
        : Mtxs(other.k) {
        for (size_t i = 0; i < k; ++i) {
            mtxs[i] = other.mtxs[i];
        }
    }

    Mtxs& operator=(const Mtxs& other) {
        if (this != &other) {
            Mtxs temp(other);

            std::swap(k, temp.k);
            std::swap(mtxs, temp.mtxs);
        }

        return *this;
    }

    Mtxs(Mtxs&& other) noexcept
        : k(other.k), mtxs(other.mtxs) {
        other.k = 0;
        other.mtxs = nullptr;
    }

    Mtxs& operator=(Mtxs&& other) noexcept {
        if (this != &other) {
            delete[] mtxs;

            k = other.k;
            mtxs = other.mtxs;

            other.k = 0;
            other.mtxs = nullptr;
        }

        return *this;
    }

    Mtx conv(Mtx m, Mtx(*t)(Mtx, Mtx));

    void erase(size_t index);
    void erase(size_t first, size_t last);
    void erase();

    void insert(size_t pos, const Mtx& value);
    void insert(size_t pos, const Mtx* values, size_t count);
    void insert(size_t pos, size_t count, const Mtx& value);

    Mtx& operator[](size_t index) {
        if (index >= k) {
            throw std::out_of_range("Invalid matrix index");
        }

        return mtxs[index];
    }

    const Mtx& operator[](size_t index) const {
        if (index >= k) {
            throw std::out_of_range("Invalid matrix index");
        }

        return mtxs[index];
    }

    size_t size() const noexcept {
        return k;
    }
};


Mtx Mtxs::conv(Mtx m, Mtx(*t)(Mtx, Mtx)) {
    if (t == nullptr) {
        throw std::invalid_argument("Null function");
    }

    for (size_t i = 0; i < k; ++i) {
        m = t(std::move(m), mtxs[i]);
    }

    return m;
}


void Mtxs::erase(size_t index) {
    if (index >= k) {
        throw std::out_of_range("Invalid index");
    }

    erase(index, index + 1);
}


void Mtxs::erase(size_t first, size_t last) {
    if (first > last || last > k) {
        throw std::out_of_range("Invalid range");
    }

    if (first == last) {
        return;
    }

    Mtxs temp(k - (last - first));
    size_t j = 0;

    for (size_t i = 0; i < k; ++i) {
        if (i < first || i >= last) {
            temp.mtxs[j] = mtxs[i];
            ++j;
        }
    }

    std::swap(k, temp.k);
    std::swap(mtxs, temp.mtxs);
}


void Mtxs::erase() {
    size_t count = 0;

    for (size_t i = 0; i < k; ++i) {
        if (mtxs[i].rows != 0 && mtxs[i].cols != 0) {
            ++count;
        }
    }

    if (count == k) {
        return;
    }

    Mtxs temp(count);
    size_t j = 0;

    for (size_t i = 0; i < k; ++i) {
        if (mtxs[i].rows != 0 && mtxs[i].cols != 0) {
            temp.mtxs[j] = mtxs[i];
            ++j;
        }
    }

    std::swap(k, temp.k);
    std::swap(mtxs, temp.mtxs);
}


void Mtxs::insert(size_t pos, const Mtx& value) {
    insert(pos, &value, 1);
}


void Mtxs::insert(size_t pos, const Mtx* values, size_t count) {
    if (pos > k) {
        throw std::out_of_range("Invalid position");
    }

    if (count == 0) {
        return;
    }

    if (values == nullptr) {
        throw std::invalid_argument("Null array");
    }

    if (count > std::numeric_limits<size_t>::max() - k) {
        throw std::length_error("Array size overflow");
    }

    Mtxs temp(k + count);

    for (size_t i = 0; i < pos; ++i) {
        temp.mtxs[i] = mtxs[i];
    }

    for (size_t i = 0; i < count; ++i) {
        temp.mtxs[pos + i] = values[i];
    }

    for (size_t i = pos; i < k; ++i) {
        temp.mtxs[i + count] = mtxs[i];
    }

    std::swap(k, temp.k);
    std::swap(mtxs, temp.mtxs);
}


void Mtxs::insert(size_t pos, size_t count, const Mtx& value) {
    if (pos > k) {
        throw std::out_of_range("Invalid position");
    }

    if (count == 0) {
        return;
    }

    if (count > std::numeric_limits<size_t>::max() - k) {
        throw std::length_error("Array size overflow");
    }

    Mtxs temp(k + count);

    for (size_t i = 0; i < pos; ++i) {
        temp.mtxs[i] = mtxs[i];
    }

    for (size_t i = 0; i < count; ++i) {
        temp.mtxs[pos + i] = value;
    }

    for (size_t i = pos; i < k; ++i) {
        temp.mtxs[i + count] = mtxs[i];
    }

    std::swap(k, temp.k);
    std::swap(mtxs, temp.mtxs);
}


Mtx add_matrices(Mtx a, Mtx b) {
    if (a.rows != b.rows || a.cols != b.cols) {
        throw std::invalid_argument("Different matrix sizes");
    }

    for (size_t i = 0; i < a.rows; ++i) {
        for (size_t j = 0; j < a.cols; ++j) {
            a.at(i, j) += b.at(i, j);
        }
    }

    return a;
}


Mtxs operator+(const Mtxs& a, const Mtxs& b) {
    if (b.k > std::numeric_limits<size_t>::max() - a.k) {
        throw std::length_error("Array size overflow");
    }

    Mtxs result(a.k + b.k);

    for (size_t i = 0; i < a.k; ++i) {
        result.mtxs[i] = a.mtxs[i];
    }

    for (size_t i = 0; i < b.k; ++i) {
        result.mtxs[a.k + i] = b.mtxs[i];
    }

    return result;
}


Mtxs operator|(const Mtxs& a, const Mtxs& b) {
    if (a.k != b.k) {
        throw std::invalid_argument("Different array sizes");
    }

    Mtxs result(a.k);

    for (size_t i = 0; i < a.k; ++i) {
        result.mtxs[i] = add_matrices(a.mtxs[i], b.mtxs[i]);
    }

    return result;
}


int main() {
    Mtxs arr(2);

    arr[0] = Mtx(2, 2);
    arr[1] = Mtx(2, 2);

    arr[0][0][0] = 5;
    arr[1][0][0] = 7;

    Mtx result = arr.conv(Mtx(2, 2), add_matrices);

    std::cout << "conv: " << result[0][0] << '\n';

    Mtxs copy = arr;
    copy[0][0][0] = 100;

    std::cout << "Original: " << arr[0][0][0] << '\n';

    Mtxs moved = std::move(copy);

    Mtx extra(2, 2);
    extra[0][0] = 1;

    arr.insert(1, extra);

    Mtx group[2] = {Mtx(2, 2), Mtx(2, 2)};
    arr.insert(0, group, 2);

    arr.insert(arr.size(), 2, extra);

    std::cout << "After insert: " << arr.size() << '\n';

    arr.erase(0);
    arr.erase(0, 2);

    arr.insert(0, Mtx());
    arr.erase();

    std::cout << "After erase: " << arr.size() << '\n';

    Mtxs combined = arr + moved;
    Mtxs summed = moved | moved;

    std::cout << "Combined size: " << combined.size() << '\n';
    std::cout << "Operator |: " << summed[0][0][0] << '\n';

    return 0;
}
