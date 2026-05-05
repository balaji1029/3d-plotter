// Week 1 — templates preview
//
// You won't write heavy template code yet — just enough to read GLM and the
// STL comfortably. A function template, then a class template.
//
// Tasks (TODO):
//   1. Implement min_of as a function template.
//   2. Implement Stack<T> as a class template using a std::vector<T> backing.

#include <cassert>
#include <iostream>
#include <vector>

template <typename T>
T min_of(const T& a, const T& b) {
    // TODO: return whichever is smaller (use operator<).
    (void)a; (void)b;
    return a;
}

template <typename T>
class Stack {
   public:
    void push(const T& v) { (void)v; /* TODO */ }
    void pop()            { /* TODO */ }
    const T& top() const  { static T dummy{}; return dummy; /* TODO */ }
    bool empty() const    { return true; /* TODO */ }
    std::size_t size() const { return 0; /* TODO */ }

   private:
    std::vector<T> data_;
};

int main() {
    std::cout << "min_of(3, 7) = " << min_of(3, 7) << "\n";
    std::cout << "min_of(2.5, 1.1) = " << min_of(2.5, 1.1) << "\n";

    Stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);
    std::cout << "stack size = " << s.size() << " top = " << s.top() << "\n";
}
