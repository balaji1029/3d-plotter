# Week 0-1: C++ refresher.
# Each exercise is its own tiny executable so you can debug them in isolation.

set(EXERCISES
    references
    raii
    smart_pointers
    operator_overloading
    function_overloading
    templates_preview
)

foreach(ex IN LISTS EXERCISES)
    add_executable(${ex} ${ex}.cpp)
endforeach()

# Suggested workflow:
#   cmake -S . -B build -DUSE_ASAN=ON
#   cmake --build build -j
#   ./build/week01_cpp/raii
#   gdb ./build/week01_cpp/raii
