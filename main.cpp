#include <iostream>
#include <string>
#include <limits>

#include "BinarySearchTree.hpp"
#include "Exceptions.hpp"
#include "ListSequence.hpp"

namespace UI {

void ClearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

template <typename T>
T ReadValue(const std::string& prompt) {
    T value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            return value;
        }
        std::cout << "[Error] Invalid input. Please try again.\n";
        ClearInput();
    }
}

template <typename T>
struct DisplayNode {
    T val;
    DisplayNode* left = nullptr;
    DisplayNode* right = nullptr;

    explicit DisplayNode(const T& v) : val(v) {}
    ~DisplayNode() {
        delete left;
        delete right;
    }
};

template <typename T>
DisplayNode<T>* ReconstructHierarchy(const BinarySearchTree<T>& tree, ListSequence<T>& elements, int leftIdx, int rightIdx) {
    if (leftIdx > rightIdx) return nullptr;

    int rootIdx = -1;
    size_t expectedSize = static_cast<size_t>(rightIdx - leftIdx + 1);

    for (int i = leftIdx; i <= rightIdx; ++i) {
        try {
            BinarySearchTree<T> sub = tree.GetSubtree(elements.Get(i));
            if (sub.GetSize() == expectedSize) {
                rootIdx = i;
                break;
            }
        } catch (...) {
            continue;
        }
    }

    if (rootIdx == -1) return nullptr;

    DisplayNode<T>* node = new DisplayNode<T>(elements.Get(rootIdx));
    node->left = ReconstructHierarchy(tree, elements, leftIdx, rootIdx - 1);
    node->right = ReconstructHierarchy(tree, elements, rootIdx + 1, rightIdx);
    return node;
}

template <typename T>
void PrintBranch(const DisplayNode<T>* node, const std::string& prefix, bool isLeft) {
    if (!node) return;

    std::cout << prefix;
    std::cout << (isLeft ? "|-- (L) " : "\\-- (R) ");
    std::cout << "[" << node->val << "]\n";

    std::string childPrefix = prefix + (isLeft ? "|   " : "    ");
    if (node->left || node->right) {
        if (node->left) {
            PrintBranch(node->left, childPrefix, true);
        } else {
            std::cout << childPrefix << "|-- (L) [null]\n";
        }

        if (node->right) {
            PrintBranch(node->right, childPrefix, false);
        } else {
            std::cout << childPrefix << "\\-- (R) [null]\n";
        }
    }
}

template <typename T>
void DrawTreeStructure(const BinarySearchTree<T>& tree) {
    if (tree.IsEmpty()) {
        std::cout << " (Tree is empty)\n";
        return;
    }

    ListSequence<T> elements;
    IEnumerator<T>* it = tree.GetEnumerator();
    while (it->MoveNext()) {
        elements.Append(it->GetCurrent());
    }
    delete it;

    DisplayNode<T>* root = ReconstructHierarchy(tree, elements, 0, elements.GetLength() - 1);

    if (root) {
        std::cout << "\n---------------- Tree Structure ----------------\n";
        std::cout << "[Root] [" << root->val << "]\n";
        if (root->left) PrintBranch(root->left, "", true);
        if (root->right) PrintBranch(root->right, "", false);
        std::cout << "------------------------------------------------\n";
        delete root;
    } else {
        std::cout << " (Unable to reconstruct hierarchy)\n";
    }

    std::cout << "[In-Order Sequence (LKP)]: ";
    for (int i = 0; i < elements.GetLength(); ++i) {
        std::cout << elements.Get(i) << " ";
    }
    std::cout << "\n";
}

void ShowMainMenu() {
    std::cout << "\n================= BST MANAGEMENT MENU =================\n";
    std::cout << " 1. Insert element\n";
    std::cout << " 2. Remove element\n";
    std::cout << " 3. Check element presence (Contains)\n";
    std::cout << " 4. Display tree structure and elements\n";
    std::cout << " 5. Clear tree\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << " 6. Extract subtree (GetSubtree)\n";
    std::cout << " 7. Check subtree presence (ContainsSubtree)\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << " 8. Filter: keep even numbers only (Where)\n";
    std::cout << " 9. Transform: multiply elements by 2 (Map)\n";
    std::cout << "10. Aggregate: calculate sum of elements (Reduce)\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << " 0. Exit\n";
    std::cout << "=======================================================\n";
}

} // namespace UI

int main() {
    BinarySearchTree<int> tree;

    // Демонстрационное наполнение
    tree.Insert(50);
    tree.Insert(30);
    tree.Insert(70);
    tree.Insert(20);
    tree.Insert(40);
    tree.Insert(60);
    tree.Insert(80);

    std::cout << ">>> Initialized sample BST with " << tree.GetSize() << " nodes.\n";
    std::cout << ">>> Initial tree structure:";
    UI::DrawTreeStructure(tree);

    bool running = true;
    while (running) {
        UI::ShowMainMenu();
        int choice = UI::ReadValue<int>("Select menu option: ");

        try {
            switch (choice) {
                case 1: {
                    int val = UI::ReadValue<int>("Enter an integer to insert: ");
                    if (tree.Insert(val)) {
                        std::cout << "-> Value " << val << " successfully inserted. Total size: " << tree.GetSize() << "\n";
                    } else {
                        std::cout << "-> Value " << val << " already exists in the tree (duplicates not allowed).\n";
                    }
                    break;
                }
                case 2: {
                    int val = UI::ReadValue<int>("Enter value to remove: ");
                    if (tree.Remove(val)) {
                        std::cout << "-> Value " << val << " removed. New size: " << tree.GetSize() << "\n";
                    } else {
                        std::cout << "-> Value " << val << " not found in the tree.\n";
                    }
                    break;
                }
                case 3: {
                    int val = UI::ReadValue<int>("Enter value to search: ");
                    if (tree.Contains(val)) {
                        std::cout << "-> Value " << val << " is present in the tree.\n";
                    } else {
                        std::cout << "-> Value " << val << " is not found.\n";
                    }
                    break;
                }
                case 4: {
                    std::cout << "\nTotal nodes: " << tree.GetSize() << "\n";
                    UI::DrawTreeStructure(tree);
                    break;
                }
                case 5: {
                    tree.Clear();
                    std::cout << "-> Tree has been cleared.\n";
                    break;
                }
                case 6: {
                    int rootVal = UI::ReadValue<int>("Enter root value of the subtree: ");
                    BinarySearchTree<int> sub = tree.GetSubtree(rootVal);
                    std::cout << "-> Subtree successfully extracted (nodes: " << sub.GetSize() << "):\n";
                    UI::DrawTreeStructure(sub);
                    break;
                }
                case 7: {
                    std::cout << "Constructing query subtree for testing.\n";
                    int count = UI::ReadValue<int>("Enter number of elements for query subtree: ");
                    BinarySearchTree<int> queryTree;
                    for (int i = 0; i < count; ++i) {
                        int el = UI::ReadValue<int>("Element #" + std::to_string(i + 1) + ": ");
                        queryTree.Insert(el);
                    }
                    if (tree.ContainsSubtree(queryTree)) {
                        std::cout << "-> Result: YES, the query tree is a valid subtree of the current tree.\n";
                    } else {
                        std::cout << "-> Result: NO, the query tree is not contained in the current tree.\n";
                    }
                    break;
                }
                case 8: {
                    auto evens = tree.Where([](const int& x) { return x % 2 == 0; });
                    std::cout << "-> Result of Where operation (even elements only):\n";
                    UI::DrawTreeStructure(evens);
                    break;
                }
                case 9: {
                    auto doubled = tree.Map<int>([](const int& x) { return x * 2; });
                    std::cout << "-> Result of Map operation (each element * 2):\n";
                    UI::DrawTreeStructure(doubled);
                    break;
                }
                case 10: {
                    if (tree.IsEmpty()) {
                        std::cout << "-> Tree is empty, sum is 0.\n";
                    } else {
                        int sum = tree.Reduce<int>(0, [](const int& acc, const int& x) {
                            return acc + x;
                        });
                        std::cout << "-> Result of Reduce operation (total sum of nodes): " << sum << "\n";
                    }
                    break;
                }
                case 0: {
                    std::cout << "Exiting application.\n";
                    running = false;
                    break;
                }
                default: {
                    std::cout << "[Warning] Unknown menu option. Please try again.\n";
                    break;
                }
            }
        } catch (const std::exception& ex) {
            std::cout << "[Exception] Operation failed: " << ex.what() << "\n";
        } catch (...) {
            std::cout << "[Unknown Error] An unexpected error occurred.\n";
        }
    }

    return 0;
}