#include <gtest/gtest.h>
#include <string>
#include "BinarySearchTree.hpp"
#include "ListSequence.hpp"
#include "Exceptions.hpp"

template <typename T>
void AssertTreeEqualsSequence(const BinarySearchTree<T>& tree, const Sequence<T>& expected) {
    ASSERT_EQ(tree.GetSize(), static_cast<size_t>(expected.GetLength()));
    
    IEnumerator<T>* it = tree.GetEnumerator();
    int index = 0;
    while (it->MoveNext()) {
        ASSERT_LT(index, expected.GetLength());
        EXPECT_EQ(it->GetCurrent(), expected.Get(index));
        ++index;
    }
    EXPECT_EQ(index, expected.GetLength());
    delete it;
}

TEST(BSTBasicsTest, EmptyTreeInitialization) {
    BinarySearchTree<int> tree;
    EXPECT_TRUE(tree.IsEmpty());
    EXPECT_EQ(tree.GetSize(), 0);
    EXPECT_FALSE(tree.Contains(10));
}

TEST(BSTBasicsTest, InsertionAndSearch) {
    BinarySearchTree<int> tree;
    EXPECT_TRUE(tree.Insert(50));
    EXPECT_TRUE(tree.Insert(30));
    EXPECT_TRUE(tree.Insert(70));
    EXPECT_EQ(tree.GetSize(), 3);

    EXPECT_FALSE(tree.Insert(50));
    EXPECT_EQ(tree.GetSize(), 3);

    EXPECT_TRUE(tree.Contains(50));
    EXPECT_TRUE(tree.Contains(30));
    EXPECT_TRUE(tree.Contains(70));
    EXPECT_FALSE(tree.Contains(999));
}

TEST(BSTBasicsTest, RemovalLeafNode) {
    BinarySearchTree<int> tree;
    tree.Insert(20);
    tree.Insert(10);
    tree.Insert(30);

    EXPECT_TRUE(tree.Remove(10));
    EXPECT_EQ(tree.GetSize(), 2);
    EXPECT_FALSE(tree.Contains(10));
    EXPECT_TRUE(tree.Contains(20));
    EXPECT_TRUE(tree.Contains(30));

    EXPECT_FALSE(tree.Remove(999));
}

TEST(BSTBasicsTest, RemovalNodeWithOneChild) {
    BinarySearchTree<int> tree;
    tree.Insert(50);
    tree.Insert(30);
    tree.Insert(20);

    EXPECT_TRUE(tree.Remove(30));
    EXPECT_EQ(tree.GetSize(), 2);
    EXPECT_FALSE(tree.Contains(30));
    EXPECT_TRUE(tree.Contains(20));
    EXPECT_TRUE(tree.Contains(50));
}

TEST(BSTBasicsTest, RemovalNodeWithTwoChildrenAndRoot) {
    BinarySearchTree<int> tree;
    tree.Insert(50);
    tree.Insert(20);
    tree.Insert(70);
    tree.Insert(60);
    tree.Insert(80);

    EXPECT_TRUE(tree.Remove(50));
    EXPECT_EQ(tree.GetSize(), 4);
    EXPECT_FALSE(tree.Contains(50));
    EXPECT_TRUE(tree.Contains(60));

    ListSequence<int> expected;
    expected.Append(20);
    expected.Append(60);
    expected.Append(70);
    expected.Append(80);

    AssertTreeEqualsSequence(tree, expected);
}

TEST(BSTBasicsTest, ClearTree) {
    BinarySearchTree<int> tree;
    tree.Insert(10);
    tree.Insert(20);
    tree.Clear();

    EXPECT_TRUE(tree.IsEmpty());
    EXPECT_EQ(tree.GetSize(), 0);
    EXPECT_FALSE(tree.Contains(10));
}

TEST(BSTTraversalTest, StrictOrderingWithRotations) {
    BinarySearchTree<int> tree;
    ListSequence<int> expected;

    for (int i = 1; i <= 7; ++i) {
        tree.Insert(i);
        expected.Append(i);
    }
    EXPECT_EQ(tree.GetSize(), 7);

    AssertTreeEqualsSequence(tree, expected);
}

TEST(BSTTraversalTest, EnumeratorExceptionOnInvalidAccess) {
    BinarySearchTree<int> tree;
    IEnumerator<int>* it = tree.GetEnumerator();
    
    EXPECT_THROW(it->GetCurrent(), EmptyContainerException);
    EXPECT_FALSE(it->MoveNext());
    EXPECT_THROW(it->GetCurrent(), EmptyContainerException);
    
    delete it;
}

TEST(BSTMemoryTest, DeepCopyConstructor) {
    BinarySearchTree<int> original;
    original.Insert(10);
    original.Insert(5);
    original.Insert(15);

    BinarySearchTree<int> copy(original);
    EXPECT_EQ(copy.GetSize(), original.GetSize());

    IEnumerator<int>* itOrig = original.GetEnumerator();
    IEnumerator<int>* itCopy = copy.GetEnumerator();
    while (itOrig->MoveNext() && itCopy->MoveNext()) {
        EXPECT_EQ(itOrig->GetCurrent(), itCopy->GetCurrent());
    }
    delete itOrig;
    delete itCopy;

    copy.Insert(100);
    original.Remove(5);
    EXPECT_FALSE(original.Contains(100));
    EXPECT_TRUE(copy.Contains(5));
}

TEST(BSTMemoryTest, MoveSemantics) {
    BinarySearchTree<int> source;
    source.Insert(1);
    source.Insert(2);

    BinarySearchTree<int> destination(std::move(source));
    EXPECT_EQ(destination.GetSize(), 2);
    EXPECT_TRUE(destination.Contains(1));
    EXPECT_TRUE(destination.Contains(2));
}

TEST(BSTFunctionalTest, WhereFiltering) {
    BinarySearchTree<int> tree;
    for (int i = 1; i <= 6; ++i) {
        tree.Insert(i);
    }

    auto evens = tree.Where([](const int& x) { return x % 2 == 0; });
    
    ListSequence<int> expected;
    expected.Append(2);
    expected.Append(4);
    expected.Append(6);

    AssertTreeEqualsSequence(evens, expected);
}

TEST(BSTFunctionalTest, MapTypeTransformation) {
    BinarySearchTree<int> tree;
    tree.Insert(1);
    tree.Insert(2);
    tree.Insert(3);

    auto string_tree = tree.Map<std::string>([](const int& x) {
        return "val_" + std::to_string(x);
    });

    ListSequence<std::string> expected;
    expected.Append("val_1");
    expected.Append("val_2");
    expected.Append("val_3");

    AssertTreeEqualsSequence(string_tree, expected);
}

TEST(BSTFunctionalTest, ReduceAccumulation) {
    BinarySearchTree<int> tree;
    tree.Insert(10);
    tree.Insert(20);
    tree.Insert(30);

    int sum = tree.Reduce<int>(0, [](const int& acc, const int& x) {
        return acc + x;
    });
    EXPECT_EQ(sum, 60);
}

TEST(BSTSubtreeTest, ExtractSubtreeSuccess) {
    BinarySearchTree<int> tree;
    tree.Insert(50);
    tree.Insert(30);
    tree.Insert(70);
    tree.Insert(20);
    tree.Insert(40);

    BinarySearchTree<int> subtree = tree.GetSubtree(30);
    EXPECT_EQ(subtree.GetSize(), 3);

    ListSequence<int> expected;
    expected.Append(20);
    expected.Append(30);
    expected.Append(40);

    AssertTreeEqualsSequence(subtree, expected);
}

TEST(BSTSubtreeTest, ExtractSubtreeNotFoundThrows) {
    BinarySearchTree<int> tree;
    tree.Insert(10);
    tree.Insert(20);

    EXPECT_THROW(tree.GetSubtree(999), InvalidArgumentException);
}

TEST(BSTSubtreeTest, ContainsSubtreePositiveAndNegative) {
    BinarySearchTree<int> tree;
    tree.Insert(50);
    tree.Insert(30);
    tree.Insert(70);
    tree.Insert(20);
    tree.Insert(40);

    BinarySearchTree<int> valid_sub = tree.GetSubtree(30);
    EXPECT_TRUE(tree.ContainsSubtree(valid_sub));

    BinarySearchTree<int> empty_sub;
    EXPECT_TRUE(tree.ContainsSubtree(empty_sub));

    BinarySearchTree<int> invalid_sub;
    invalid_sub.Insert(30);
    invalid_sub.Insert(20);
    invalid_sub.Insert(99);
    EXPECT_FALSE(tree.ContainsSubtree(invalid_sub));
}