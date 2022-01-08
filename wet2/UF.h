#ifndef UF_H
#define UF_H

#include "Group.h"
#include "RankTree.h"
#include <memory>

class UF {
    private:
        int k;
        int* parent;
        int* size;

    public:
        Group* groups;
        UF(int k, int scale) : k(k), parent(new int[k]), size(new int[k]), groups(new Group[k]) {
            for(int i=0; i<k; ++i) {
                parent[i] = i;
                size[i] = 1;
                groups[i] = Group(i, scale);
            }
        }
        
        ~UF() {
            delete[] parent;
            delete[] groups;
            delete[] size;
        }

        int Size() {
            return k;
        }

        int Find(int id) {
            int current = id;
            while(parent[current] != current) {
                current = parent[current];
            }
            int shrink = id;
            while(parent[shrink] != shrink) {
                int temp = parent[shrink]   ;
                parent[shrink] = current;
                shrink = temp;
            }
            return current;
        }

        int Union(int group1, int group2) {
            // assums group1, group2 are roots of different groups
            if(size[group1] > size[group2]) {
                parent[group2] = group1;
                size[group1] += size[group2];
                groups[group1].merge(groups[group2]);
                return group1;
            }
            parent[group1] = group2;
            size[group2] += size[group1];
            groups[group2].merge(groups[group1]);
            return group2;   
        }
};

#endif