#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <iostream>
#include <memory>
#include <exception>

/** 
 * NOTE: this is non-const BST, meaning key and info are not saved as const.
 * this is because when deleting a node we overwrite their values.
 * if this is important this should be dealt with (for example, using std::swap)
*/

template<class Key, class Info>
class BST {
    protected:
        struct Node {
            Key key;
            Info info;
            int height = 0;
            std::shared_ptr<Node> left;
            std::shared_ptr<Node> right;
            Node(const Key& key, const Info& info, const int height=0,
                std::shared_ptr<Node> left=nullptr, std::shared_ptr<Node> right=nullptr)
                    : key(key), info(info), height(height), left(left), right(right) { }
            Node(const Node* other) {
                key = other->key;
                info = other->info;
                height = other->height;
                left = other->left;
                right = other->right;
            }
        };
        std::shared_ptr<Node> root = nullptr;


        static int height_aux(std::shared_ptr<Node>& root) {
            if(!root) {
                return 0;
            }
            return root->height;
        }

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
                root = std::make_shared<Node>(key, info);
                return;
            }
            if(key < root->key) {
                insert_aux(root->left, key, info);
            }
            else if(key > root->key) {
                insert_aux(root->right, key, info);
            }
            else {  // key == root->key, should never happen if called from find()
                throw std::invalid_argument("key already exists.");
            }
            // int left_height = root->left ? root->left->height : 0;
            // int right_height = root->right ? root->right->height : 0;
            root->height = 1 + std::max(height_aux(root->left), height_aux(root->right));
            return;
        }

        static void remove_aux(std::shared_ptr<Node>& root, const Key key) {
            if(!root) {
                return;
            }
            if(key < root->key) {
                remove_aux(root->left, key);
                root->height--;
            }
            else if(key > root->key) {
                remove_aux(root->right, key);
                root->height--;
            }
            else {  // key == root->key
                // root has no leafs
                if(!root->left && !root->right) {
                    root.reset();  // shared_ptr should dispose of the pointer
                }
                // root has only left leaf
                else if(!root->right) {
                    root = root->left;  
                    root->left.reset();  // shared_ptr should dispose of pointer
                }
                // root has only right leaf
                else if(!root->left) {
                    root = root->right;
                    root->right.reset();  // shared_ptr should dispose of pointer
                }
                // root has two leafs
                else {
                    std::shared_ptr<Node> next = root->right;
                    while(next->left) {
                        next = next->left;
                    }
                    Key key = next->key;
                    Info info = next->info;
                    remove_aux(root, next->key);
                    root->key = key;
                    root->info = info;
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
            to = std::make_shared<Node>(from->key, from->info);
            to->left = deep_copy(from->left, to->left);
            to->right = deep_copy(from->right, to->right);
            return to;
        }

        static void clear_aux(std::shared_ptr<Node>& root) {
            if(!root) {
                return;
            }
            clear_aux(root->left);
            root->left.reset();
            clear_aux(root->right);
            root->right.reset();
            root.reset();
        }

    public:
        BST() : root(nullptr) { }
        BST(Key key, Info info) : root(std::make_shared<Node>(key, info)) { }
        BST(const BST& other) {
            this->root = deep_copy(other.root, this->root);
        }
        BST& operator=(const BST& other) {
            if(this->root == other.root) {
                return *this;
            }
            this->root = deep_copy(other.root, this->root);
            return *this;
        }
        ~BST() {
            clear_aux(root);
        }

        std::shared_ptr<Node> find(const Key key) const {
            return find_aux(root, key);
        }
        void insert(const Key key, const Info info) {
            if(!find(key)) {
                insert_aux(root, key, info);
            }
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
        
        int height(Key key) {
            std::shared_ptr<Node> node = find(key);
            if(node) {
                return height_aux(node);
            }
            return -1;
        }
};

#endif