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
 class EmptyTree : public std::exception { 
    public:
        virtual const char* what() const noexcept override {
            return "Can't perform operation on an empty tree.";
        }
 };

template<class Key, class Info>
struct Node {
    Key key;
    Info info;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    int height = 0;

    Node(const Key& key, Info info) : 
        key(key), info(info), left(nullptr), right(nullptr), height(0) { }
    Node(const Node& other) : key(other.key), info(other.info), left(other.left), right(other.right), height(height) { }
    // ~Node() {
    //     info = nullptr;
    //     left.reset();
    //     right.reset();
    // }
};
template<class Key, class Info>
class AVL {
    typedef Node<Key, Info> _Node;
    private:
        static int height(std::shared_ptr<_Node>& root) {
            if(!root) {
                return -1;
            }
            return root->height;
        }

        static int get_balance_factor(std::shared_ptr<_Node>& root) {
            return height(root->left) - height(root->right);
        }

        static void RR_rotation(std::shared_ptr<_Node>& parent) {
            std::shared_ptr<_Node> child = parent->right;
            parent->right = child->left;
            parent->height = 1 + std::max(height(parent->left), height(parent->right));
            child->height = 1 + std::max(height(child->right), height(parent));
            child->left = parent;
            parent = child;
        }
        static void RL_rotation(std::shared_ptr<_Node>& parent) {
            LL_rotation(parent->right);
            RR_rotation(parent);
        }
        static void LL_rotation(std::shared_ptr<_Node>& parent)
        {
            std::shared_ptr<_Node> child = parent->left;
            std::shared_ptr<_Node> tmp = child->right;
            child->right = parent;
            parent->left = tmp;
            parent->height = 1 + std::max(height(parent->left), height(parent->right));
            child->height = 1 + std::max(height(child->left), height(child->right));
            parent = child;
        }
        static void LR_rotation(std::shared_ptr<_Node>& parent) {
            RR_rotation(parent->left);
            LL_rotation(parent);
        }

        static void balance_aux(std::shared_ptr<_Node>& root) {
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

        static std::shared_ptr<_Node>& find_aux(std::shared_ptr<_Node>& root, const Key& key) {
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

        static void insert_aux(std::shared_ptr<_Node>& root, const Key& key, Info info) {
            if(!root) {
                root = std::make_shared<_Node>(key, info);
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

        static void remove_aux(std::shared_ptr<_Node>& root, Key key) {
            if(!root) {
                return;
            }
            if(key < root->key) {
                remove_aux(root->left, key);
                root->height = 1 + std::max(height(root->left), height(root->right));
            }
            else if(root->key < key) {
                remove_aux(root->right, key);
                root->height = 1 + std::max(height(root->left), height(root->right));
            }
            else {  // key == root->key
                // root is a leaf
                if(!root->left && !root->right) {
                    root = nullptr;//.reset();
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
                    std::shared_ptr<_Node> next = root->right;
                    while(next->left) {
                        next = next->left;
                    }
                    // std::shared_ptr<Node> temp = deep_copy(next, temp);
                    // temp->left = root->left;
                    // temp->right = root->right;
                    // root = deep_copy(temp, root);
                    root->key = next->key;
                    root->info = next->info;
                    // temp.clear();
                    remove_aux(root->right, root->key);
                }
            }
            balance_aux(root);
        }

        static Info& getMaxAux(std::shared_ptr<_Node>& root) {
            if(!root->right) {
                return root->info;
            }
            return getMaxAux(root->right);
        }
        static Info& getMinAux(std::shared_ptr<_Node>& root) {
            if(!root->left) {
                return root->info;
            }
            return getMaxAux(root->left);
        }

        static void clear_aux(std::shared_ptr<_Node>& root) {
            if(!root) {
                return;
            }
            clear_aux(root->left);
            clear_aux(root->right);
            // root->info.reset();// = nullptr;
            // root->left = nullptr;
            // root->right = nullptr;
            root.reset();
        }

        static std::shared_ptr<_Node> deep_copy(const std::shared_ptr<_Node>& from, std::shared_ptr<_Node>& to) {
            if(!from) {
                return nullptr;
            }
            to = std::make_shared<_Node>(from->key, from->info);
            to->left = deep_copy(from->left, to->left);
            to->right = deep_copy(from->right, to->right);
            return to;
        }

        /* merge helper functions */
        static void inorderToList(std::shared_ptr<_Node>& root, Array<_Node*>& list) {
            if(!root) {
                return;
            }
            inorderToList(root->left, list);
            list.push_back(root.get());
            inorderToList(root->right, list);
        }
        Array<_Node*> getTreeAsList() {
            Array<_Node*> list(number_of_nodes);
            inorderToList(root, list);  // O(n)
            return list; 
        }
        class NodesComparator {
            public:
                bool operator()(_Node* left, _Node* right) {
                    return left->key < right->key;
                }
        };
        /** 
         * getTreeFomListAux: creates a balanced binary tree (AVL) from a list in O(n), n being the length of the list:
         * no need for rotations and searching, each node get assinged a value such that there are balanced number
         * of nodes from each side, using a proccess similliar to binary search.
        */
        static std::shared_ptr<_Node> getTreeFromListAux(Array<_Node*>& list, int start, int end) {
            if(start > end) {
                return nullptr;
            }
            int mid = (start+end)/2;
            std::shared_ptr<_Node> root;
            root = std::make_shared<_Node>(list[mid]->key, list[mid]->info);
            // root = std::shared_ptr<Node>(list[mid]);
            // free(list[mid]);
            root->left = getTreeFromListAux(list, start, mid-1);
            root->right = getTreeFromListAux(list, mid+1, end);
            return root;
        }
        static AVL getTreeFromList(Array<_Node*>& list) {
            AVL avl;
            avl.root = getTreeFromListAux(list, 0, list.getSize());
            avl.number_of_nodes = list.getSize()+1;
            // list.clean();
            return avl;
        }

    public:
        std::shared_ptr<_Node> root;
        int number_of_nodes = 0;
        AVL() = default;
        AVL(Key key, Info info) : root(std::make_shared<_Node>(key, info)) { }
        AVL(const AVL& other) {
            this->root = deep_copy(other.root, this->root);
            this->number_of_nodes = other.number_of_nodes;
        }
        ~AVL() {
            if(number_of_nodes > 0)
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
        void copyRoot(const AVL& from) {
            root = from.root;
        }
        std::shared_ptr<_Node>& find(const Key& key) {
            return find_aux(root, key);
        }
        Info& getInfo(const Key& key) {
            std::shared_ptr<_Node> node = find(key);
            if(!node) {
                throw KeyDoesNotExist();
            }
            return node->info;
        }

        void insert(const Key& key, Info& info) {
            if(find(key)) {
                throw KeyAlreadyExists();
            }
            // std::shared_ptr<Info> info_ptr = std::shared_ptr<Info>(&info);
            insert_aux(root, key, info);
            number_of_nodes++;
        }
        void remove(const Key& key) {
            if(!find(key)) {
                throw KeyDoesNotExist();
            }
            remove_aux(root, key);
            number_of_nodes--;
        }

        Info& getMax() {
            if(!root) {
                throw EmptyTree();
            }
            return getMaxAux(root);
        }
        Info& getMin() {
            if(!root) {
                throw EmptyTree();
            }
            return getMinAux(root);
        }

        // static Info& getInfo(std::shared_ptr<_Node>& node) {
        //     return node->info;
        // }
        // static Key& getKey(std::shared_ptr<_Node>& node) {
        //     return node->key;
        // }

        void clear() {
            clear_aux(root);
        }

        int get_tree_height() {
            return height(root);
        }

        static void print_inorder(std::shared_ptr<_Node>& root) {
            if(!root) {
                return;
            }
            print_inorder(root->left);
            std::cout << (root->key) << ", ";
            print_inorder(root->right);
        }
        void inorder() {
            print_inorder(root);
            std::cout << std::endl;
        }
        static void print_preorder(std::shared_ptr<_Node>& root) {
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
        static void print_postorder(std::shared_ptr<_Node>& root) {
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

        static Array<_Node*> mergeToList(AVL& avl1, AVL& avl2) {
            NodesComparator cmp;
            Array<_Node*> list1 = avl1.getTreeAsList();  // O(n)
            Array<_Node*> list2 = avl2.getTreeAsList();  // O(m)
            Array<_Node*> merged_list = Array<_Node*>::merge(list1, list2, cmp);  // O(n+m)
            return merged_list;
        }

        static AVL listToAVL(Array<_Node*>& list) {
            // std::cout << merged_list;
            AVL avl = getTreeFromList(list);  // O(n+m)
            // return getTreeFromList(merged_list);  // O(n+m)
            return avl;
            // counting copying and destructing lists and trees (if performed), the overall complexity is still O(n+m)
        }
        static AVL merge(AVL& avl1, AVL& avl2) {
            Array<_Node*> merged_list = mergeToList(avl1, avl2);
            return listToAVL(merged_list);
        }
};

#endif