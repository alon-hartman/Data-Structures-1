#ifndef DynamicHashTable_H
#define DynamicHashTable_H

#include "Player.h"
#include <memory>
#include <exception>


class PlayerDoesntExists : public std::exception {
    public:
        virtual const char* what() const noexcept override {
            return "Player does not exists";
        }
};

class PlayerAlreadyExists : public std::exception {
    public:
        virtual const char* what() const noexcept override {
            return "Player already exists";
        }
};

struct Node {
    std::shared_ptr<Player> data;
    Node* next;
    Node() : data(nullptr), next(nullptr) { };
    Node(std::shared_ptr<Player> player, Node* next) : data(player), next(next) { }
    ~Node() {
        data = nullptr;
        while(next) {
            delete next;
        }
    }
};

class LinkedList {
    public:
        int size;
        Node* head;
        LinkedList() : size(0), head(nullptr) { };
        ~LinkedList() {
            delete head;
        }
        void insert(std::shared_ptr<Player> player) {
            Node* new_node = new Node(player, head);
            head = new_node;
            size++;
        }
        void remove(std::shared_ptr<Player> player) {
            if(head == nullptr) {
                throw PlayerDoesntExists();
            };
            if(player->playerID == head->data->playerID) {
                // remove from top of list
                Node* garbage = head;
                head = head->next;
                delete garbage;
                size--;
                return;
            }
            Node* temp = head;
            while(temp->next != nullptr && temp->next->data->playerID != player->playerID) {
                temp = temp->next;
            }
            Node* garbage = temp->next;
            if(garbage == nullptr) {
                throw PlayerDoesntExists();
            };
            temp->next = garbage->next;
            delete garbage;
            size--;
        }
        std::shared_ptr<Player> findPlayer(int PlayerID)
        {
            Node* iterator = head;
            while(iterator != nullptr && iterator->data->playerID != PlayerID) {
                iterator = iterator->next;
            }
            if(iterator != nullptr) {
                return iterator->data;
            }
            return nullptr;
        }
};

class DHT {
    private:
        LinkedList* players;
        
        int hashFunction(int i, int m) {
            return (i>=0 ) ? i % m : m + i%m;
        }
    public:
        int size;
        int number_of_players;
        DHT(int size=2) : size(size), number_of_players(0), players(new LinkedList[size]) { }

        ~DHT() {
            delete[] players;
        }

        DHT& operator=(const DHT& other) {
            if(this == &other) {
                return *this;
            }
            delete[] players;
            this->players = other.players;
            this->size = other.size;
            this->number_of_players = other.number_of_players;
        }

        void swap(DHT& other) {
            LinkedList* temp = players;
            players = other.players;
            other.players = temp;
            int _temp = size;
            size = other.size;
            other.size = _temp;
            _temp = number_of_players;
            number_of_players = other.number_of_players;
            other.number_of_players = _temp;

        }

        int getSize() {
            return size;
        }

        void addPlayer(const std::shared_ptr<Player>& player) {
            if(findPlayer(player->playerID) != nullptr)
            {
                throw PlayerAlreadyExists();
            }
            players[hashFunction(player->playerID, size)].insert(player);
            number_of_players++;
            if(number_of_players > size/2) {
                resize(size*2);
            }
        }

        void removePlayer(std::shared_ptr<Player> player) {
            players[hashFunction(player->playerID, size)].remove(player);
            number_of_players--;
            if(number_of_players < size/4) {
                resize(size/2);
            }
        }

        std::shared_ptr<Player> findPlayer(int PlayerID) {
            std::shared_ptr<Player> player = players[hashFunction(PlayerID, size)].findPlayer(PlayerID);
            return player;
        }

        void resize(int new_size) {
            LinkedList* new_players = new LinkedList[new_size];
            for(int i=0; i<size; ++i) {
                Node* iterator = players[i].head;
                while(iterator != nullptr) {
                    new_players[hashFunction(iterator->data->playerID, new_size)].insert(iterator->data);
                    iterator = iterator->next;
                }
            }
            LinkedList* temp = players;
            players = new_players;
            delete[] temp;
            size = new_size;
        }

         static DHT* merge(DHT& dht1, DHT& dht2) {
             DHT* merged = new DHT(dht1.size + dht2.size);
             for(int i=0; i<dht1.size; ++i) {
                Node* iterator = dht1.players[i].head;
                while(iterator) {
                    merged->addPlayer(iterator->data);
                    iterator = iterator->next;
                }
             }
             for(int i=0; i<dht2.size; ++i) {
                Node* iterator = dht2.players[i].head;
                while(iterator) {
                    merged->addPlayer(iterator->data);
                    iterator = iterator->next;
                }
             }
             return merged;
         }
};

#endif