// Week 1 — references vs pointers
//
// Goals:
//   - Reference must be initialized; pointer can be null/reseated.
//   - `const T&` is the default way to pass non-trivial parameters.
//   - Returning a reference vs returning a value vs returning a pointer.
//
// Tasks (TODO):
//   1. Implement swap_by_ref so the assertions below pass.
//   2. Implement clamp_in_place using a reference parameter.
//   3. Make biggest() return a const reference into the input vector.

#include <cassert>
#include <iostream>
#include <vector>

void swap_by_ref(int& a, int& b) {
    // TODO: swap a and b without using std::swap.
    (void)a; (void)b;
}

void clamp_in_place(int& x, int lo, int hi) {
    // TODO: if x < lo set x = lo; if x > hi set x = hi.
    (void)x; (void)lo; (void)hi;
}

const int& biggest(const std::vector<int>& v) {
    // TODO: return a reference to the largest element. Assume v is non-empty.
    return v.front();
}

int main() {
    int a = 3, b = 7;
    swap_by_ref(a, b);
    assert(a == 7 && b == 3);

    int x = 50;
    clamp_in_place(x, 0, 10);
    assert(x == 10);

    std::vector<int> v{1, 9, 4, 9, 2};
    assert(biggest(v) == 9);

    std::cout << "references: all checks passed\n";
}
