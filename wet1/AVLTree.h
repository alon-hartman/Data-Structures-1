#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <memory>
#include <exception>
#include <iostream>
#include "Array.h"


/*     EXCEPTIONS     */
class KeyAlreadyExists : public std::exception { 
    public:
        virtual const char* what() const noexcept override {
            return "Key already exists";
        }
};
class KeyDoesNotExist : public std::exception { 
    public:
        virtual const char* what() const noexcept override {
            return "Key does not exist";
        }
 };

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
                this->key = other.key;
                this->info = other.info;
                this->left = other.left;
                this->right = other.right;
                this->height = other.height;
                return *this;
            }
            // void destroyNode(std::shared_ptr<Node>& node) {
            //     if(!node) {
            //         return;
            //     }
            //     destroyNode(node->left);
            //     destroyNode(node->right);
            //     node.reset();
            // }
            // ~Node() {
            //     destroyNode(this->left);
            //     destroyNode(this->right);  // ..?
            // };
        };
        std::shared_ptr<Node> root;
        int number_of_nodes = 0;

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

        static void clear_aux(std::shared_ptr<Node>& root) {
            if(!root) {
                return;
            }
            clear_aux(root->left);
            clear_aux(root->right);
            root.reset();
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

        /* merge helper functions */
        static void inorderToList(std::shared_ptr<Node>& root, Array<std::shared_ptr<Node>>& list) {
            if(!root) {
                return;
            }
            inorderToList(root->left, list);
            list.push_back(root);
            inorderToList(root->right, list);
        }
        Array<std::shared_ptr<Node>> getTreeAsList() {
            Array<std::shared_ptr<Node>> list(number_of_nodes);
            inorderToList(root, list);  // O(n)
            return list; 
        }
        class NodesComparator {
            public:
                bool operator()(std::shared_ptr<Node> left, std::shared_ptr<Node> right) {
                    return left->key < right->key;
                }
        };
        /** 
         * getTreeFomListAux: creates a balanced binary tree (AVL) from a list in O(n), n being the length of the list:
         * no need for rotations and searching, each node get assinged a value such that there are balanced number
         * of nodes from each side, using a proccess similliar to binary search.
        */
        static std::shared_ptr<Node> getTreeFromListAux(Array<std::shared_ptr<Node>>& list, int start, int end) {
            if(start > end) {
                return nullptr;
            }
            int mid = (start+end)/2;
            std::shared_ptr<Node> root;
            root = list[mid];//std::make_shared<Node>(list[mid]->key, list[mid]->info);
            root->left = getTreeFromListAux(list, start, mid-1);
            root->right = getTreeFromListAux(list, mid+1, end);
            return root;
        }
        static AVL getTreeFromList(Array<std::shared_ptr<Node>>& list) {
            AVL avl;
            avl.root = getTreeFromListAux(list, 0, list.getSize());
            avl.number_of_nodes = list.getSize()+1;
            // list.clean();
            std::cout << avl.root.use_count() << std::endl;
            return avl;
        }

    public:
        AVL() = default;
        AVL(Key key, Info info) : root(std::make_shared<Node>(key, info)) { }
        AVL(const AVL& other) {
            this->root = other.root;//deep_copy(other.root, this->root);
            this->number_of_nodes = other.number_of_nodes;
        }
        ~AVL() {
            clear_aux(root);  // stack overflow?
        }
        AVL& operator=(const AVL& other) {
            if(this == &other) {
                return *this;
            }
            // this->root = other.root;
            this->root = deep_copy(other.root, this->root);
            this->number_of_nodes = other.number_of_nodes;
            return *this;
        }

        std::shared_ptr<Node>& find(const Key key) {
            return find_aux(root, key);
        }

        void insert(Key key, Info info) {
            if(find(key)) {
                throw KeyAlreadyExists();
            }
            insert_aux(root, key, info);
            number_of_nodes++;
        }
        void remove(Key key) {
            if(!find(key)) {
                throw KeyDoesNotExist();
            }
            remove_aux(root, key);
            number_of_nodes--;
        }

        static Info& getInfo(std::shared_ptr<Node>& node) {
            return node->info;
        }
        static Key& getKey(std::shared_ptr<Node>& node) {
            return node->key;
        }

        void clear() {
            clear_aux(root);
        }

        int get_tree_height() {
            return height(root);
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

        static AVL merge(AVL& avl1, AVL& avl2) {
            NodesComparator cmp;
            Array<std::shared_ptr<Node>> list1 = avl1.getTreeAsList();  // O(n)
            Array<std::shared_ptr<Node>> list2 = avl2.getTreeAsList();  // O(m)
            Array<std::shared_ptr<Node>> merged_list = Array<std::shared_ptr<Node>>::merge(list1, list2, cmp);  // O(n+m)
            // std::cout << merged_list;
            return getTreeFromList(merged_list);  // O(n+m)
            // counting copying and destructing lists and trees (if performed), the overall complexity is still O(n+m)
        }
};

#endif