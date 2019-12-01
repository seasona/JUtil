#include <algorithm>
#include <cstdio>
#include <iostream>

class dummy_array {
public:
    dummy_array(std::size_t size = 0)
        : size_(size), my_array_(size_ ? new int[size_]() : nullptr) {
        printf("default construct\n");
    }
    dummy_array(const dummy_array& other) {
        printf("copy construct\n");
    }

    // use default construct first, then move construct
    dummy_array(dummy_array&& other) : dummy_array() {
        printf("move construct\n");
        swap(*this, other);
    }

    // use copy construct first, then copy construct operator
    dummy_array& operator=(dummy_array other) {
        printf("copy construct operator\n");
        swap(*this, other);
        return *this;
    }

    // dummy_array& operator=(dummy_array&& other) {
    //     printf("move construct operator\n");
    //     swap(*this, other);
    //     return *this;
    // }

    void swap(dummy_array& first, dummy_array& second) {
        std::swap(first.my_array_, second.my_array_);
        std::swap(first.size_, second.size_);
    }

    ~dummy_array() { delete[] my_array_; }

    std::size_t size() { return size_; }

private:
    // use initizalize list will start from the statement, so the size_ must
    // declare before my_array_
    std::size_t size_;
    int* my_array_;
};

int main() {
    dummy_array f(5);
    dummy_array g(f);
    std::cout << f.size() << std::endl;
    std::cout << g.size() << std::endl;
    //g = f;
    //f = std::move(g);
    return 0;
}

// result:
// default construct
// default construct
// copy construct
// copy construct operator
// default construct
// move construct
// copy construct operator