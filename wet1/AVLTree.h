#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <memory>
#include <exception>
#include <iostream>

template<class Key, class Info>
class AVL {
    private:
        struct Node {
            Key key;
            Info info;
            std::shared_ptr<Node> left;
            std::shared_ptr<Node> right;
            int height = 0;

            Node(Key key, Info info) : key(key), info(info), left(nullptr), right(nullptr), height(0) { }
            Node(const Node& other) : key(other.key), info(other.info), left(other.left), right(other.right), height(height) { }
            Node& operator=(const Node& other) {
                if(*this == other) {
                    return *this;
                }
                return Node(other);
            }
            ~Node() = default;
        };
        std::shared_ptr<Node> root;

        static int height(std::shared_ptr<Node>& root) {
            if(!root) {
                return -1;
            }
            return root->height;
        }

        static int get_balance_factor(std::shared_ptr<Node>& root) {
            return height(root->left) - height(root->right);
        }

        static void RR_rotation(std::shared_ptr<Node>& parent) {
            std::shared_ptr<Node> child = parent->right;
            parent->right = child->left;
            parent->height = 1 + std::max(height(parent->left), height(parent->right));
            child->height = 1 + std::max(height(child->right), height(parent));
            child->left = parent;
            parent = child;
        }
        static void RL_rotation(std::shared_ptr<Node>& parent) {
            LL_rotation(parent->right);
            RR_rotation(parent);
        }
        static void LL_rotation(std::shared_ptr<Node>& parent)
        {
            std::shared_ptr<Node> child = parent->left;
            std::shared_ptr<Node> tmp = child->right;
            child->right = parent;
            parent->left = tmp;
            parent->height = 1 + std::max(height(parent->left), height(parent->right));
            child->height = 1 + std::max(height(child->left), height(child->right));
            parent = child;
        }
        static void LR_rotation(std::shared_ptr<Node>& parent) {
            RR_rotation(parent->left);
            LL_rotation(parent);
        }

        static void balance_aux(std::shared_ptr<Node>& root) {
            // if(key == root->key) {
            //     return;
            // }
            // else if(key < root->key) {
            //     balance_tree_aux(root->left, key);
            // }
            // else {  // key > root->key
            //     balance_tree_aux(root->right, key);
            // }
            if(!root) {
                return;
            }

            int balance_factor = get_balance_factor(root);
            if(balance_factor == 2) {
                if(get_balance_factor(root->left) >= 0) {
                    LL_rotation(root);
                }
                else if (get_balance_factor(root->left) == -1) {
                    LR_rotation(root);
                }
            }
            else if(balance_factor == -2) {
                if(get_balance_factor(root->right) <= 0) {
                    RR_rotation(root);
                }
                else if(get_balance_factor(root->right) == 1) {
                    RL_rotation(root);
                }
            }
        }

        static std::shared_ptr<Node>& find_aux(std::shared_ptr<Node>& root, Key key) {
            if(!root || key == root->key) {
                return root;
            }
            else if(key < root->key) {
                return find_aux(root->left, key);
            }
            else {  //(key > root->key) {
                return find_aux(root->right, key);
            }
        }

        static void insert_aux(std::shared_ptr<Node>& root, Key key, Info info) {
            if(!root) {
                root = std::make_shared<Node>(key, info);
                return;
            }
            else if(key < root->key) {
                insert_aux(root->left, key, info);
            }
            else {  // key > root->key
                insert_aux(root->right, key, info);
            }
            balance_aux(root);
            root->height = 1 + std::max(height(root->left), height(root->right));
        }

        static void remove_aux(std::shared_ptr<Node>& root, Key key) {
            if(!root) {
                return;
            }
            if(key < root->key) {
                remove_aux(root->left, key);
                root->height = 1 + std::max(height(root->left), height(root->right));
            }
            else if(key > root->key) {
                remove_aux(root->right, key);
                root->height = 1 + std::max(height(root->left), height(root->right));
            }
            else {  // key == root->key
                // root is a leaf
                if(!root->left && root->right) {
                    root.reset();
                }
                // root has left child
                else if(!root->right) {
                    root = root->left;
                    // root->left.reset();
                }
                // root has right child
                else if(!root->left) {  
                    root = root->right;
                }
                // root has both childs
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
            balance_aux(root);
        }

        static std::shared_ptr<Node> deep_copy(const std::shared_ptr<Node>& from, std::shared_ptr<Node>& to) {
            if(!from) {
                return nullptr;
            }
            to = std::make_shared<Node>(from->key, from->info);
            // to = std::shared_ptr<Node>(new Node(key, info))
            to->left = deep_copy(from->left, to->left);
            to->right = deep_copy(from->right, to->right);
            return to;
        }

    public:
        AVL() = default;
        AVL(Key key, Info info) : root(std::make_shared<Node>(key, info)) { }
        AVL(const AVL& other) {
            this->root = deep_copy(other.root, this->root);
        }
        AVL& operator=(const AVL& other) {
            if(this == &other) {
                return *this;
            }
            this->root = deep_copy(other.root, this->root);
            return *this;
        }

        std::shared_ptr<Node>& find(const Key key) {
            return find_aux(root, key);
        }
        void insert(Key key, Info info) {
            if(find(key)) {
                throw std::invalid_argument("Key already exist in tree");
            }
            insert_aux(root, key, info);
        }
        // std::shared_ptr<Node>& remove(Key key);
        void remove(Key key) {
            if(!find(key)) {
                throw std::invalid_argument("Key does not exist in tree");
            }
            remove_aux(root, key);
        }

        static void print_inorder(std::shared_ptr<Node>& root) {
            if(!root) {
                return;
            }
            print_inorder(root->left);
            std::cout << root->key << ", ";
            print_inorder(root->right);
        }
        void inorder() {
            print_inorder(root);
            std::cout << std::endl;
        }
        static void print_preorder(std::shared_ptr<Node>& root) {
            if(!root) {
                return;
            }
            std::cout << root->key << ", ";
            print_preorder(root->left);
            print_preorder(root->right);
        }
        void preorder() {
            print_preorder(root);
            std::cout << std::endl;
        }
        static void print_postorder(std::shared_ptr<Node>& root) {
            if(!root) {
                return;
            }
            print_postorder(root->left);
            print_postorder(root->right);
            std::cout << root->key << ", ";
        }
        void postorder() {
            print_postorder(root);
            std::cout << std::endl;
        }
        
};



#endif