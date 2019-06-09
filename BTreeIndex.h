//
//  BTreeIndex.h
//  BPlusTreeIndex
//
//  Created by qinyiliu&yuqianhou on 5/19/19.
//  Copyright © 2019 qinyiliu&yuqianhou. All rights reserved.
//
#include <iostream>
#include <cstring>
#include <algorithm>
#include <sys/mman.h>
using namespace std;

#ifndef BTreeIndex_h
#define BTreeIndex_h

// To define Record struct
struct Record{
    char value[6][15];
};

// To define buffer
void* buffer[20];
int buf_cnt;

// Char to int
int ctoi(char *s){
    int cnt = 0;
    for (int i = 0; i < strlen(s); ++i)
        cnt = cnt * 10 + s[i] - '0';
    return cnt;
}

// To define Node struct
struct Node{
    void *child[7];
    int v[6], num;
    bool isleaf, isroot;
    Node(){
        num = 0;
        for (int i = 1; i <= 5; ++i)
            child[i] = NULL;
        isleaf = isroot = false;
    }
    void add(void* record, int vv){
        v[++num] = vv;
        child[num] = record;
        for (int i = 1; i <= num; ++i)
            for (int j = 1; j < num; ++j)
                if (v[j] > v[j+1]){
                    swap(v[j], v[j+1]);
                    swap(child[j], child[j+1]);
                }
    }
    void split_leaf(){
        buffer[buf_cnt++] = mmap(NULL, sizeof(Node), PROT_READ|PROT_WRITE, MAP_PRIVATE, 0, 0);
        Node* newpage = (Node*)buffer[buf_cnt-1];//store into buffer[]
        newpage->v[1] = v[4];
        newpage->v[2] = v[5];
        newpage->child[1] = child[4];
        newpage->child[2] = child[5];//[1,2,3,4,5] -> [1,2,3] [4,5]
        newpage->num = 2;
        num = 3;
        newpage->isleaf = true;
        child[5] = newpage;//in leafnode of b+tree, the final pointer points at its sibling
        child[4] = NULL;
    }
    void Insert(void *record, int vv, bool &splited, int &k, void* p){
        buffer[buf_cnt++] = this;
        if (isleaf){
            add(record, vv);
            if (num == 4){
                split_leaf();
                Node* newpage = (Node*)buffer[buf_cnt-1];
                buf_cnt--;
                splited = true;
                p = newpage;
                k = newpage->v[1];
                return;
            }
            splited = false;
            return;
        }
        int pl = 0;
        for (int i = 1; i <= num; ++i)
            if (vv < v[i]) {
                pl = i;
                buffer[buf_cnt++] = child[i];
                Node* now = (Node*) buffer[buf_cnt-1];
                now->Insert(record, vv, splited, k, p);
            }
        if (vv >= v[num]){
            pl = num + 1;
            buffer[buf_cnt++] = child[num+1];
            Node* now = (Node*)buffer[buf_cnt-1];
            now->Insert(record, vv, splited, k, p);
        }
        if (splited){
            for (int i = num + 1; i > pl; ++i)
            {
                child[i+1] = child[i];
                v[i] = v[i-1];
            }
            child[pl+1] = p;
            v[pl] = k;
            if (num == 4){//still need to split here
                splited = true;
                Node* sib = (Node*)mmap(NULL, sizeof(Node), PROT_READ|PROT_WRITE, MAP_PRIVATE, 0, 0);
                sib->num = 2;
                sib->child[1] = child[4];
                sib->child[2] = child[5];
                sib->child[3] = child[6];
                sib->v[1] = v[4];
                sib->v[2] = v[5];
                num = 2;//[1,2,3,4,5] -> [1,2] [4,5] update([3], p')
                k = v[3];
                p = sib;
            }
            splited = false;
            return;
        }
        splited = false;
        return;
    }
    
    // Look up
    bool Lookup(char *str){
        int vv = ctoi(str);
        if (isleaf){
            for (int i = 0; i < num; ++i)
                if (vv == v[i]) {
                    Record* rec = (Record*)child[i];
                    for (int j = 0; j < 6; ++j)
                        cout << rec->value[j] << ' ';
                    cout << endl;
                    return true;
                }
            return false;
        }
        for (int i = 0; i < num; ++i)
            if (vv <= v[i]) {
                Node* now = (Node*)child[i];
                return now->Lookup(str);
            }
        Node* now = (Node*)child[num];
        return now->Lookup(str);
        //there is no need of buffer in lookup operation?
    }
    
};


class BTreeIndex {
public:
    Node* root;
    BTreeIndex();
    ~BTreeIndex();
    bool Lookup(char *str);
    void Insert(void* record, int vv);
};

#endif /* BTreeIndex_h */
