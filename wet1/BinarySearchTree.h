#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <iostream>
#include <memory>
#include <exception>

template<class Key, class Info>
class BST {
    protected:
        struct Node {
            Key key;
            Info info;
            std::shared_ptr<Node> left;
            std::shared_ptr<Node> right;
            Node(const Key& key, const Info& info, std::shared_ptr<Node> left=nullptr, std::shared_ptr<Node> right=nullptr) : 
                key(key), info(info), left(left), right(right) { }
            Node(const Node* other) {
                key = other->key;
                info = other->info;
                left = other->left;
                right = other->right;
            }
        };
        std::shared_ptr<Node> root = nullptr;


        static std::shared_ptr<Node> find_aux(std::shared_ptr<Node> root, const Key key) {
            if(!root || root->key == key) {
                return root;
            }
            else if(key < root->key) {
                return find_aux(root->left, key);
            }
            else {  // key > root->key
                return find_aux(root->right, key);
            }
        }

        static void insert_aux(std::shared_ptr<Node>& root, const Key key, const Info info) {
            if(!root) {
                root = std::make_shared<Node>(new Node(key, info));
                return;
            }
            if(key < root->key) {
                insert_aux(root->left, key, info);
                return;
            }
            else if(key > root->key) {
                insert_aux(root->right, key, info);
                return;
            }
            else {  // key == root->key
                throw std::invalid_argument("key already exists.");
                return;
            }
        }

        static void remove_aux(std::shared_ptr<Node>& root, const Key key) {
            if(!root) {
                return;
            }
            if(key < root->key) {
                remove_aux(root->left, key);
            }
            else if(key > root->key) {
                remove_aux(root->right, key);
            }
            else {  // key == root->key
                // root has no leafs
                if(!root->left && !root->right) {
                    root = nullptr;  // shared_ptr should dispose of the pointer
                }
                // root has only left leaf
                else if(!root->right) {
                    root = root->left;  
                    root->left = nullptr;  // shared_ptr should dispose of pointer
                }
                // root has only right leaf
                else if(!root->left) {
                    root = root->right;
                    root->right = nullptr;  // shared_ptr should dispose of pointer
                }
                // root has two leafs
                else {
                    std::shared_ptr<Node> next = root->right;
                    while(next->left) {
                        next = next->left;
                    }
                    root->key = next->key;
                    root->info = next->info;
                    remove_aux(root->right, next->key);
                }
            }
            return;
        }

        // TRAVERSE TECHNIQUES
        static void traverse_preorder(std::shared_ptr<Node>& root, std::ostream& os) {
            if(!root) {
                return;
            }
            std::cout << root->key << ", ";
            traverse_preorder(root->left, os);
            traverse_preorder(root->right, os);
        }
        static void traverse_inorder(std::shared_ptr<Node>& root, std::ostream& os) {
            if(!root) {
                return;
            }
            traverse_inorder(root->left, os);
            std::cout << root->key << ", ";
            traverse_inorder(root->right, os);
        }
        static void traverse_postorder(std::shared_ptr<Node>& root, std::ostream& os) {
            if(!root) {
                return;
            }
            traverse_postorder(root->left, os);
            traverse_postorder(root->right, os);
            std::cout << root->key << ", ";
        }

        static std::shared_ptr<Node> deep_copy(const std::shared_ptr<Node>& from, std::shared_ptr<Node>& to) {
            if(!from) {
                return nullptr;
            }
            to = std::shared_ptr<Node>(new Node(from->key, from->info));
            to->left = deep_copy(from->left, to->left);
            to->right = deep_copy(from->right, to->right);
            // other->key = root->key;
            // other->info = root->info;
            // if(root->left) {
            //     other->left = std::make_shared<Node>(new Node(root->left->key, root->left->info));
            //     deep_copy(root->left, other->left);
            // }
            // if(root->right) {
            //     other->right = std::make_shared<Node>(new Node(root->right->key, root->right->info));
            //     deep_copy(root->left, other->left);
            // }
            return to;
        }

    public:
        BST() : root(nullptr) { }
        BST(Key key, Info info) : root(std::make_shared<Node>(new Node(key, info))) { }
        BST(const BST& other) {
            this->root = deep_copy(other.root, this->root);
        }
        BST& operator=(const BST& other) {
            this->root = deep_copy(other.root, this->root);
        }

        /** 
         * TODO: copy c'tor, operator=.
        */
        std::shared_ptr<Node> find(const Key key) const {
            return find_aux(root, key);
        }
        void insert(const Key key, const Info info) {
            insert_aux(root, key, info);
        }
        void remove(Key key) {
            remove_aux(root, key);
        }

        void print_preorder(std::ostream& os = std::cout) {
            traverse_preorder(root, os);
            os << std::endl;
        }
        void print_inorder(std::ostream& os = std::cout) {
            traverse_inorder(root, os);
            os << std::endl;
        }
        void print_postorder(std::ostream& os = std::cout) {
            traverse_postorder(root, os);
            os << std::endl;
        }
};

#endif