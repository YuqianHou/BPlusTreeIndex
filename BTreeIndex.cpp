//
//  BTreeIndex.cpp
//  BPlusTreeIndex
//
//  Created by qinyiliu&yuqianhou on 5/19/19.
//  Copyright © 2019 qinyiliu&yuqianhou. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <sys/mman.h>
#include "BTreeIndex.h"

using namespace std;

BTreeIndex::BTreeIndex(){
    root = new Node();
    root->isroot = true;
    root->isleaf = true;
}

BTreeIndex::~BTreeIndex(){
    delete root;
}

bool BTreeIndex::Lookup(char *str){
    return root->Lookup(str);
}
void BTreeIndex::Insert(void* record, int vv){
    
    bool splited = false;
    int k;
    void* p;
    buf_cnt = 0;
    root->Insert(record, vv, splited, k, p);
    //root = (Node*)buffer[0];
    if (splited){
        Node* newroot =(Node*) mmap(NULL, sizeof(Node), PROT_READ|PROT_WRITE, MAP_PRIVATE, 0, 0);
        newroot->num = 1;
        newroot->v[1] = k;
        newroot->child[1] = root;
        newroot->child[2] = p;
        newroot->isroot = true;
        root->isroot = false;
        root = newroot;
    }
}
