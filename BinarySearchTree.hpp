#ifndef _BINARYSEARCHTREE_HPP_
#define _BINARYSEARCHTREE_HPP_

#include <cstddef>
#include <memory>
#include <utility>
#include <functional>
#include "IEnumerator.hpp"
#include "Stack.hpp"
#include "Exceptions.hpp"


template <typename T>
class BinarySearchTree;

// TreeEnumerator
template <typename T>
class TreeEnumerator : public IEnumerator<T> {
private:
    using NodeType = typename BinarySearchTree<T>::Node;

    const NodeType* root;
    const NodeType* current_node;
    Stack<const NodeType*> traversal_stack;

    void PushLeftBranch(const NodeType* node) {
        while (node != nullptr) {
            traversal_stack.Push(node);
            node = node->left.get();
        }
    }

public:
    explicit TreeEnumerator(const NodeType* root_node)
        : root(root_node), current_node(nullptr) {
        Reset();
    }

    bool MoveNext() override {
        if (traversal_stack.IsEmpty()) {
            current_node = nullptr;
            return false;
        }

        current_node = traversal_stack.Pop();

        if (current_node->right) {
            PushLeftBranch(current_node->right.get());
        }

        return true;
    }

    const T& GetCurrent() const override {
        if (!current_node) {
            throw EmptyContainerException("Enumerator has no current element or is out of bounds");
        }
        return current_node->data;
    }

    T& GetCurrent() override {
        if (!current_node) {
            throw EmptyContainerException("Enumerator has no current element or is out of bounds");
        }
        return const_cast<T&>(current_node->data);
    }

    void Reset() override {
        current_node = nullptr;
        traversal_stack.Clear();
        PushLeftBranch(root);
    }

    IEnumerator<T>* Clone() const override {
        auto* copy = new TreeEnumerator<T>(root);
        copy->current_node = this->current_node;
        copy->traversal_stack = this->traversal_stack;
        return copy;
    }
};

// BinarySearchTree
template <typename T>
class BinarySearchTree : public IEnumerable<T> {
    friend class TreeEnumerator<T>;

private:
    struct Node {
        T data;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        int height;

        explicit Node(const T& val)
            : data(val), left(nullptr), right(nullptr), height(1) {}
    };

    std::unique_ptr<Node> root;
    size_t count;

    static int Max(int a, int b) {
        return (a > b) ? a : b;
    }

    int GetHeight(const std::unique_ptr<Node>& n) const {
        return n ? n->height : 0;
    }

    int GetBalanceFactor(const std::unique_ptr<Node>& n) const {
        return n ? GetHeight(n->left) - GetHeight(n->right) : 0;
    }

    void UpdateHeight(Node* n) {
        if (n) {
            n->height = 1 + Max(GetHeight(n->left), GetHeight(n->right));
        }
    }

    std::unique_ptr<Node> RotateRight(std::unique_ptr<Node> y) {
        std::unique_ptr<Node> x = std::move(y->left);
        y->left = std::move(x->right);
        UpdateHeight(y.get());

        x->right = std::move(y);
        UpdateHeight(x.get());
        return x;
    }

    std::unique_ptr<Node> RotateLeft(std::unique_ptr<Node> x) {
        std::unique_ptr<Node> y = std::move(x->right);
        x->right = std::move(y->left);
        UpdateHeight(x.get());

        y->left = std::move(x);
        UpdateHeight(y.get());
        return y;
    }

    std::unique_ptr<Node> Balance(std::unique_ptr<Node> node) {
        if (!node) return nullptr;
        UpdateHeight(node.get());
        int balance = GetBalanceFactor(node);

        if (balance > 1) {
            if (GetBalanceFactor(node->left) < 0) {
                node->left = RotateLeft(std::move(node->left));
            }
            return RotateRight(std::move(node));
        }
        if (balance < -1) {
            if (GetBalanceFactor(node->right) > 0) {
                node->right = RotateRight(std::move(node->right));
            }
            return RotateLeft(std::move(node));
        }
        return node;
    }

    std::unique_ptr<Node> Insert(std::unique_ptr<Node> node, const T& item) {
        if (!node) {
            ++count;
            return std::make_unique<Node>(item);
        }
        if (item < node->data) {
            node->left = Insert(std::move(node->left), item);
        } else if (node->data < item) {
            node->right = Insert(std::move(node->right), item);
        }
        return Balance(std::move(node));
    }

    std::unique_ptr<Node> ExtractMin(std::unique_ptr<Node>& node) {
        if (!node->left) {
            std::unique_ptr<Node> min_node = std::move(node);
            node = std::move(min_node->right);
            return min_node;
        }
        std::unique_ptr<Node> min_node = ExtractMin(node->left);
        node = Balance(std::move(node));
        return min_node;
    }

    std::unique_ptr<Node> Remove(std::unique_ptr<Node> node, const T& item) {
        if (!node) return nullptr;

        if (item < node->data) {
            node->left = Remove(std::move(node->left), item);
        } else if (node->data < item) {
            node->right = Remove(std::move(node->right), item);
        } else {
            --count;
            if (!node->left) {
                return std::move(node->right);
            }
            if (!node->right) {
                return std::move(node->left);
            }

            std::unique_ptr<Node> successor = ExtractMin(node->right);
            successor->left = std::move(node->left);
            successor->right = std::move(node->right);
            node = std::move(successor);
        }
        return Balance(std::move(node));
    }

    std::unique_ptr<Node> CopyNodes(const std::unique_ptr<Node>& node) const {
        if (!node) return nullptr;
        auto newNode = std::make_unique<Node>(node->data);
        newNode->height = node->height;
        newNode->left = CopyNodes(node->left);
        newNode->right = CopyNodes(node->right);
        return newNode;
    }

    std::unique_ptr<Node> CopyNodes(const Node* node) const {
        if (!node) return nullptr;
        auto newNode = std::make_unique<Node>(node->data);
        newNode->height = node->height;
        newNode->left = CopyNodes(node->left.get());
        newNode->right = CopyNodes(node->right.get());
        return newNode;
    }

    bool Contains(const Node* node, const T& item) const {
        if (!node) return false;
        if (item < node->data) return Contains(node->left.get(), item);
        if (node->data < item) return Contains(node->right.get(), item);
        return true;
    }

    const Node* FindNode(const Node* node, const T& item) const {
        if (!node) return nullptr;
        if (item < node->data) return FindNode(node->left.get(), item);
        if (node->data < item) return FindNode(node->right.get(), item);
        return node;
    }

    size_t CountNodes(const Node* node) const {
        if (!node) return 0;
        return 1 + CountNodes(node->left.get()) + CountNodes(node->right.get());
    }

    bool AreIdentical(const Node* a, const Node* b) const {
        if (!a && !b) return true;
        if (!a || !b) return false;

        bool values_equal = !(a->data < b->data) && !(b->data < a->data);

        return values_equal && 
               AreIdentical(a->left.get(), b->left.get()) && 
               AreIdentical(a->right.get(), b->right.get());
    }

public:
    BinarySearchTree() : root(nullptr), count(0) {}

    BinarySearchTree(const BinarySearchTree& other) : root(nullptr), count(other.count) {
        root = CopyNodes(other.root);
    }

    BinarySearchTree(BinarySearchTree&& other) noexcept = default;
    BinarySearchTree& operator=(BinarySearchTree&& other) noexcept = default;

    BinarySearchTree& operator=(const BinarySearchTree& other) {
        if (this != &other) {
            root = CopyNodes(other.root);
            count = other.count;
        }
        return *this;
    }

    ~BinarySearchTree() override = default;

    size_t GetSize() const { return count; }
    bool IsEmpty() const { return count == 0; }

    void Clear() {
        root.reset();
        count = 0;
    }

    bool Contains(const T& item) const {
        return Contains(root.get(), item);
    }

    bool Insert(const T& item) {
        if (Contains(item)) return false;
        root = Insert(std::move(root), item);
        return true;
    }

    bool Remove(const T& item) {
        if (!Contains(item)) return false;
        root = Remove(std::move(root), item);
        return true;
    }

    IEnumerator<T>* GetEnumerator() const override {
        return new TreeEnumerator<T>(root.get());
    }

    template <typename U>
    BinarySearchTree<U> Map(std::function<U(const T&)> func) const {
        BinarySearchTree<U> result;
        IEnumerator<T>* enumerator = this->GetEnumerator();

        while (enumerator->MoveNext()) {
            result.Insert(func(enumerator->GetCurrent()));
        }

        delete enumerator;
        return result;
    }

    BinarySearchTree<T> Where(std::function<bool(const T&)> predicate) const {
        BinarySearchTree<T> result;
        IEnumerator<T>* enumerator = this->GetEnumerator();

        while (enumerator->MoveNext()) {
            const T& current = enumerator->GetCurrent();
            if (predicate(current)) {
                result.Insert(current);
            }
        }

        delete enumerator;
        return result;
    }

    template <typename Accumulator>
    Accumulator Reduce(const Accumulator& initial, 
                       std::function<Accumulator(const Accumulator&, const T&)> func) const {
        Accumulator acc = initial;
        IEnumerator<T>* enumerator = this->GetEnumerator();

        while (enumerator->MoveNext()) {
            acc = func(acc, enumerator->GetCurrent());
        }

        delete enumerator;
        return acc;
    }

    BinarySearchTree<T> GetSubtree(const T& root_value) const {
        const Node* target_node = FindNode(root.get(), root_value);
        if (!target_node) {
            throw InvalidArgumentException("Subtree root element not found in the tree");
        }

        BinarySearchTree<T> sub_tree;
        sub_tree.root = CopyNodes(target_node);
        sub_tree.count = CountNodes(sub_tree.root.get());
        return sub_tree;
    }

    bool ContainsSubtree(const BinarySearchTree<T>& sub_tree) const {
        if (sub_tree.IsEmpty()) {
            return true;
        }

        if (sub_tree.GetSize() > this->GetSize()) {
            return false;
        }
        const Node* candidate = FindNode(root.get(), sub_tree.root->data);
        if (!candidate) {
            return false;
        }

        return AreIdentical(candidate, sub_tree.root.get());
    }
};


#endif //_BINARYSEARCHTREE_HPP_