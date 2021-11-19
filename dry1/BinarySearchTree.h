/** TODO:
 * add exception handling
*/

#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <memory>

template <class Key, class Info>
class BST {
    protected:
        class Node {
            public:
                Key key;
                Info info;
                std::unique_ptr<BST> left;
                std::unique_ptr<BST> right;
                Node() = default;
                Node(Key key, Info info, std::unique_ptr<BST> left, std::unique_ptr<BST> right) = default;
                ~Node() = default;
        }
        std::unique_ptr<Node> root = nullptr;
        
        std::unique_ptr<Node>& find_node(std::unique_ptr<Node>& node, Key& key) const {
            if(!node || node->key == key) {
                return node;
            }
            else if(key < node->key) {
                return find_node(node->left, key);
            }
            else {
                return find_node(node->right, key);
            }
        }

        void insert_aux(std::unique_ptr<Node>& node, Key key, Info info) {
            std:unique_ptr<Node> exists = find_node(node, key);
            if(exists) {
                return;
            }
            if(!node) {
                node = std::unique_ptr<Node>(new Node(key, info, nullptr, nullptr));
                return 1;
            }
            else if (key < root->key) {
                insert_aux(node->left, key, info);
                return;
            }
            else {
                insert_aux(node->right, key, info);
                return;
            }
        }

        void remove_aux(std::unique_ptr<Node>& node, Key key) {
            if(!node) {
                return;
            }
            else if(key < node->key) {
                remove_aux(node->left, key);
                return;
            }
            else if(key > node->key) {
                remove_aux(node->right, key);
                return;
            }
            else {  // key = node->key
                // no children - leaf
                if(!node->left && !node->right) {
                    delete node;
                    node = nullptr;
                }
                // only left child
                else if(!node->right) {
                    std::unique_ptr<Node> temp = node;
                    node = node->left;
                    delete temp;
                }
                // only right child
                else if(!node->left) {
                    std::unique_ptr<Node> temp = node;
                    node = node->right;
                    delete temp;
                }
                // two childrens
                else {
                    std::unique_ptr<Node> next = node->right;
                    while(next->left) {
                        next = next->left;
                    }
                    node->key = next->key;
                    node->info = next->info;
                    remove_aux(next, next);
                }
            }
        }

    public:
        BST() = default; // constructor. creates the root with default values of Key, Info (assuming they have it).
        ~BST() = default;  // destructor
        /** TODO:
        BST(const BST& other);  // copy constructor
        BST& operator=(const BST& other);
        */

        // TOURING TECHNIQUES
        void print_preorder(std::ostream& os = std::cout, std::unique_ptr<Node> node) {
            if(!node) {
                return;
            }
            os << this.key;
            if(this->right || this->left) {
                os << ", ";
            }
            print_preorder(os, node->left);
            print_preorder(os, node->right);
        }
        void print_inorder(std::ostream& os = std::cout, std::unique_ptr<Node> node) {
        if(!node) {
            return;
        }
        print_inorder(os, this->left);
        os << this.key;
        if(this->right || this->left) {
            os << ", ";
        }
        print_inorder(this->right);
    }
        void print_postorder(std::ostream& os = std::cout, std::unique_ptr<Node> node) {
        if(!node) {
            return;
        }
        print_inorder(os, this->left);
        print_inorder(this->right);
        os << this.key;
        if(this->right || this->left) {
            os << ", ";
        }
    }

        std::unique_ptr<Node>& find(Key& key) {
            return find_node(key, root);
        }

        void insert(Key key, Info info) {
            insert_from_node(root, key, info);
            return;
        }

        void remove(Key key) {
            remove_aux(root, key);
        }
};

#endif