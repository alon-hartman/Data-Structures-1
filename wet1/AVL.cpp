// #include "BinarySearchTree.h"
#include "AVLTree.h"
#include <iostream>

void test1LL() {
    std::cout << "testing LL " << std::endl;
    AVL<int, int> avl;
    avl.insert(12,1);
    avl.insert(8,1);
    avl.insert(15,1);
    avl.insert(6,1);
    avl.insert(10,1);
    avl.insert(14,1);
    avl.insert(24,1);
    avl.insert(4,1);
    avl.insert(11,1);
    avl.insert(13,1);
    avl.insert(20,1);
    avl.insert(29,1);
    avl.insert(19,1);
    
    avl.preorder();
    avl.inorder();
    avl.postorder();
    std::cout << std::endl;

    avl.insert(18,1);
    
    avl.preorder();
    avl.inorder();
    avl.postorder();
    std::cout << std::endl;
}

void test1LR() {
    std::cout << "testing LR " << std::endl;
    AVL<int, int> avl;
    avl.insert(15,1);
    avl.insert(10,1);
    avl.insert(20,1);
    avl.insert(6,1);
    avl.insert(13,1);
    avl.insert(17,1);
    avl.insert(24,1);
    avl.insert(4,1);
    avl.insert(7,1);
    avl.insert(12,1);
    avl.insert(14,1);
    
    avl.preorder();
    avl.inorder();
    avl.postorder();
    std::cout << std::endl;

    avl.insert(11,1);
    
    avl.preorder();
    avl.inorder();
    avl.postorder();
    std::cout << std::endl;
}
void test1RL() {
    std::cout << "testing RL " << std::endl;
    AVL<int, int> avl;
    avl.insert(11,1);
    avl.insert(8,1);
    avl.insert(19,1);
    avl.insert(6,1);
    avl.insert(10,1);
    avl.insert(13,1);
    avl.insert(25,1);
    avl.insert(12,1);
    avl.insert(17,1);
    avl.insert(23,1);
    avl.insert(29,1);
    
    avl.preorder();
    avl.inorder();
    avl.postorder();
    std::cout << std::endl;

    avl.insert(15,1);
    
    avl.preorder();
    avl.inorder();
    avl.postorder();
    std::cout << std::endl;
}
void test2() {
    // deleting
    std::cout << "testing delete" << std::endl;
    AVL<int, int> avl;
    avl.insert(2, 1);
    avl.insert(4, 1);
    avl.insert(3, 1);
    avl.insert(6, 1);
    avl.insert(11, 1);

    avl.remove(2);
    avl.preorder();  // should be 6, 3, 4, 11
    avl.inorder();  // should be 3, 4, 6, 11
    std::cout << std::endl;

    avl.insert(2, 1);
    avl.remove(6);
    avl.preorder();  // should be 3, 2, 11, 4
    avl.inorder();  // should be 2, 3, 4, 11
    std::cout << std::endl;

    avl.insert(6, 1);
    avl.insert(5, 1);
    avl.remove(3);
    avl.preorder();  // should be 4, 2, 6, 5, 11
    avl.inorder();  // should be 2, 4, 5, 6, 11
    std::cout << std::endl;
}

void test3() {
    AVL<int, int> avl;
    std::cout << "testing edge cases for insert, delete" << std::endl;
    try { 
        avl.remove(1);
    }
    catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    try {
        avl.insert(1, 1);
    }
    catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    try { 
        avl.remove(2);
    }
    catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    try {
        avl.insert(1, 1);
    }
    catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    try {
        avl.insert(1, 2);
    }
    catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

void test4() {
    AVL<int, int> avl;
    avl.insert(114, 2);
    avl.insert(11, 2);
    avl.insert(0, 2);
    avl.insert(113, 2);
    avl.insert(87, 2);
    avl.insert(14, 2);
    avl.insert(1134, 2);
    avl.insert(12, 2);
    avl.insert(4, 2);
    auto e = avl.find(113);
    std::cout << e->key << ", " << e->info << std::endl;
    e = avl.find(0);
    std::cout << e->key << ", " << e->info << std::endl;
    e = avl.find(4);
    std::cout << e->key << ", " << e->info << std::endl;
    std::cout << (avl.find(1) != nullptr) << std::endl;
}

void test5() {
    AVL<int, int> avl;
    avl.insert(114, 2);
    avl.insert(11, 2);
    avl.insert(0, 2);
    avl.insert(113, 2);
    avl.insert(87, 2);
    avl.insert(14, 2);
    avl.insert(1134, 2);
    avl.insert(12, 2);
    avl.insert(4, 2);

    using namespace std;
    AVL<int, int> copycon = avl;  // AVL<int, int> hasama(avl);
    copycon.remove(11);
    std::cout << ((avl.find(11) == nullptr) ? string("not found") : string("found")) << std::endl; // false
    std::cout << ((copycon.find(11) == nullptr) ? string("not found") : string("found")) << std::endl; // true
    
    // operator=
    AVL<int, int> other2;
    other2.insert(234, 0);
    other2 = avl;
    
    std::cout << ((avl.find(234) == nullptr) ? string("not found") : string("found")) << std::endl;  // true
    std::cout << ((other2.find(234) == nullptr) ? string("not found") : string("found")) << std::endl;  // true
    std::cout << ((other2.find(14) == nullptr) ? string("not found") : string("found")) << std::endl;  // false

    other2 = other2;
    std::cout << ((other2.find(14) == nullptr) ? string("not found") : string("found")) << std::endl;  // false
}


int main() {
    test1LL();
    test1LR();
    test1RL();
    test2();
    test3();
    test4();
    test5();
}