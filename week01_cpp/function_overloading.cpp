// Week 1 — function overloading
//
// Same name, different parameter lists. The compiler picks at compile time
// based on argument types.
//
// Tasks (TODO):
//   1. Add overloads of `area` for Circle, Rectangle, Triangle.
//   2. Notice how a Triangle whose base is `int` triggers the int overload of
//      a templated helper unless you're careful — a preview of next file.

#include <cmath>
#include <iostream>

struct Circle    { float r; };
struct Rectangle { float w, h; };
struct Triangle  { float base, height; };

float area(Circle c)    { (void)c; return 0.0f; }   // TODO
float area(Rectangle r) { (void)r; return 0.0f; }   // TODO
float area(Triangle t)  { (void)t; return 0.0f; }   // TODO

int main() {
    std::cout << "circle:    " << area(Circle{1.0f}) << "\n";
    std::cout << "rectangle: " << area(Rectangle{3.0f, 4.0f}) << "\n";
    std::cout << "triangle:  " << area(Triangle{6.0f, 4.0f}) << "\n";
}
