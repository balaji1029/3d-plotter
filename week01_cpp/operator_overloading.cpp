// Week 1 — operator overloading
//
// Build a tiny Vec3 you'll reuse mentally when reading GLM later.
//
// Tasks (TODO):
//   1. Implement +, -, * (scalar), unary -.
//   2. Implement dot() and cross() as free or member functions.
//   3. Stream operator<< for printing.

#include <cassert>
#include <cmath>
#include <iostream>

struct Vec3 {
    float x, y, z;

    Vec3 operator+(const Vec3& o) const {
        // TODO
        (void)o;
        return {0, 0, 0};
    }
    Vec3 operator-(const Vec3& o) const {
        (void)o;
        return {0, 0, 0};
    }
    Vec3 operator*(float s) const {
        (void)s;
        return {0, 0, 0};
    }
    Vec3 operator-() const { return {0, 0, 0}; }
};

float dot(const Vec3&, const Vec3&) {
    return 0.0f;  // TODO
}

Vec3 cross(const Vec3&, const Vec3&) {
    return {0, 0, 0};  // TODO
}

std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

int main() {
    Vec3 a{1, 2, 3};
    Vec3 b{4, 5, 6};

    auto c = a + b;
    std::cout << "a + b = " << c << "\n";

    // When the TODOs are done, these should pass:
    // assert(c.x == 5 && c.y == 7 && c.z == 9);
    // assert(dot(a, b) == 32.0f);
    // assert((cross(Vec3{1,0,0}, Vec3{0,1,0}).z) == 1.0f);
}
