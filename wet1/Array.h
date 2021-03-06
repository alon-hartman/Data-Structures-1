#ifndef ARRAY_H
#define ARRAY_H

#include <exception>
#include <iostream>

// This class implements an array which can only be populated using push_back.
// Implemented with an array and not a linked list due to simpler design.

class ArrayIsFull : public std::exception {
    public:
        const char* what() const noexcept override {
            return "Array is Full.";
        }
};

template<class T>
class Array {
    private:
        T* data;
        int size;
        int next_empty;
    public:
        // Array() : data(nullptr), size(0), next_empty(0) { }
        Array(int size) : data(new T[size]), size(size), next_empty(0) { }
        ~Array() {
            delete[] data;
        }
        void clean() {
            delete[] data;
        }
        Array(const Array& other) {
            size = other.size;
            next_empty = other.next_empty;
            data = new T[size];
            for(int i=0; i<size; ++i) {
                data[i] = other[i];
            }
        }
        Array(const Array& other, int len) {
            size = std::min(other.size, len);
            next_empty = std::min(other.next_empty, size);
            data = new T[size];
            for(int i=0; i<size; ++i) {
                data[i] = other[i];
            }
        }
        int getSize() const {
            return size-1;
        }
        T* getData() {
            return data;
        }
        Array& operator=(const Array& other) {
            if(data == other.data) {
                return *this;
            }
            delete[] data;
            size = other.size;
            next_empty = other.next_empty;
            data = new T[size];
            for(int i=0; i<size; ++i) {
                data[i] = other[i];
            }
            return *this;
        }

        T operator[](int i) const {
            return data[i];
        }

        void push_back(const T& element) {
            if(next_empty == size) {
                throw ArrayIsFull();
            }
            data[next_empty++] = element;  // copies element (i think)
        }

        template<class Compare = std::less<T>>
        static Array merge(const Array& list1, const Array& list2, Compare cmp) {
            // only merges initiallized parts
            Array merged(list1.next_empty + list2.next_empty);
            int i=0, j=0;
            while(i<list1.next_empty && j<list2.next_empty) {
                if(cmp(list1[i], list2[j])) {
                    merged.push_back(list1[i++]);
                }
                else {
                    merged.push_back(list2[j++]);
                }
            }
            while(i<list1.next_empty) merged.push_back(list1[i++]);
            while(j<list2.next_empty) merged.push_back(list2[j++]);
            return merged;
        }
};

#endif