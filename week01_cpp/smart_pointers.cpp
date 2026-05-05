// Week 1 — smart pointers
//
// unique_ptr: single owner, moves only.
// shared_ptr: shared ownership via reference count.
// weak_ptr:   non-owning observer; breaks shared_ptr cycles.
//
// Tasks (TODO):
//   1. Build a tree of Nodes using shared_ptr for children, weak_ptr for parent.
//   2. Confirm with ASan that destroying the root frees the whole tree.

#include <iostream>
#include <memory>
#include <vector>

struct Node {
    int value;
    std::weak_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;

    explicit Node(int v) : value(v) {}
    ~Node() { std::cout << "  ~Node(" << value << ")\n"; }
};

std::shared_ptr<Node> make_tree() {
    // TODO: build a small tree:
    //   node 1 has children 2 and 3
    //   node 2 has child 4
    //   each child's `parent` weak_ptr points back at its parent
    // Return a shared_ptr to node 1.
    return nullptr;
}

int main() {
    {
        auto root = make_tree();
        std::cout << "tree built\n";
        // root goes out of scope here -> all nodes should print their dtor message.
    }
    std::cout << "after scope\n";

    // unique_ptr quick demo
    auto u = std::make_unique<int>(42);
    std::unique_ptr<int> u2 = std::move(u);
    std::cout << "unique value: " << *u2 << " (u is " << (u ? "still alive" : "empty") << ")\n";
}
