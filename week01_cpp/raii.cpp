// Week 1 — RAII (Resource Acquisition Is Initialization)
//
// Idea: a resource (file, GL buffer, lock, allocation) is owned by an object;
// the destructor releases it. No leaks even on exceptions or early return.
//
// Tasks (TODO):
//   1. Finish FileHandle: open in ctor, close in dtor, disable copy, allow move.
//   2. Run under ASan (-DUSE_ASAN=ON) to confirm no leaks.

#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <utility>

class FileHandle {
   public:
    FileHandle(const char* path, const char* mode) {
        f_ = std::fopen(path, mode);
        if (!f_) throw std::runtime_error("fopen failed");
    }

    ~FileHandle() {
        // TODO: close f_ if non-null.
    }

    // TODO: delete copy constructor and copy assignment.
    // TODO: implement move constructor and move assignment that null out the source.

    std::FILE* get() const { return f_; }

   private:
    std::FILE* f_ = nullptr;
};

int main() {
    {
        FileHandle h("/tmp/raii_demo.txt", "w");
        std::fprintf(h.get(), "hello raii\n");
    }  // file should be closed here

    {
        FileHandle h("/tmp/raii_demo.txt", "r");
        char buf[64] = {};
        std::fgets(buf, sizeof(buf), h.get());
        std::cout << "read back: " << buf;
    }
}
