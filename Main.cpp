//
//  Main.cpp
//  BPlusTreeIndex
//
//  Created by qinyiliu&yuqianhou on 6/7/19.
//  Copyright © 2019 qinyiliu&yuqianhou. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <sys/mman.h>
#include "BTreeIndex.h"
using namespace std;


// To creat an array of Record
Record record[100000];


// Help document
void help(){
    //cout << ""
    cout << "insert:" << endl;
    cout << "You are supposed to enter 6 elements in each line and seperated by a space."<< endl;
    cout << "lookup:" << endl;
    cout << "You are supposed to enter an EmpID which is a number." << endl;
    cout << endl;
    cout << "If you are still confused about the input format, please read \"README\" file carefully." << endl;
    
}


int main(int argc, char* argv[]){
    BTreeIndex* T = new BTreeIndex();
    
    string filename;
    if (argc > 1) filename = string(argv[1]);
    else {
        cout << "Please enter the filename in correct format." << endl;
        return 1;
    }
    // 将CSV文件里的数据存到主存里
    ifstream infile(filename);
    string line;
    int k = 0;
    while (getline(infile, line)) {
        k++;
        istringstream input(line);
        string command;
        for (int i = 0; i < 6; i++) {
            getline(input, command, ',');
            for (int j = 0; j < command.length(); j++) {
                record[k].value[i][j] = command[j];
            }
        }
    }
    
    // To put record into disk
    int i = 0;
    while(i < k){
        Record *page = (Record*)mmap(NULL, sizeof(Record)*40, PROT_READ|PROT_WRITE, MAP_PRIVATE, 0, 0);
        for (int j = 0; j < 40 && i < k; ++i, ++j)
        {
            *(page + j) = record[i];
            int key = ctoi(record[i].value[0]);
            T->Insert(*(page+j), key);
        }
    }
    
    // To read command in a line
    cout << "Please enter your command(insert/lookup/help): " << endl;
    while (getline(cin, line)) {
        istringstream input(line);
        string command;
        getline(input, command, ' ');
        if (command == "insert") {
            // To judge if the input is correct.
            int i = 0;
            while (getline(input, command, ' ')) {
                i++;
            }
            // If correct, input the values.
            if (i == 5) {
                // To allocate new space for new record
                istringstream input2(line);
                getline(input2, command, ' ');
                Record* newrecord = new Record();
                while (getline(input, command, ' ')) {
                    for(int j = 0; j < command.length(); j++){
                        newrecord->value[i][j] = command[j];
                    }
                }
                if (!T->Lookup(newrecord->value[0])) {
                    int key = ctoi(newrecord->value[0]);
                    void* tmp = mmap(NULL, sizeof(Record), PROT_READ|PROT_WRITE, MAP_PRIVATE, 0, 0); //？？？
                    Record* now = (Record*)tmp;
                    now = newrecord;
                    T->Insert(tmp, key);
                }
                else{
                    // The existent record will be shown here.
                    cout << "This is an existent record. You can't insert it again." << endl;
                }
                delete Record;
                
            }else{
                cout << "Invalid input! Please enter command in correct format." << endl;
                cout << "If you are confused with input format, please read \"README\" file or enter \"help\" for help." << endl;
            }
            
        }else if (command == "lookup"){
            while (getline(input, command, ' ')) {
                // To judge if the input is correct.
                int i = 0;
                while (getline(input, command, ' ')) {
                    i++;
                }
                // If correct, look up the value.
                if (i == 1) {
                    istringstream input2(line);
                    getline(input2, command, ' ');
                    while (getline(input, command, ' ')) {
                        char temp[command.length()];
                        for(int j = 0; j < command.length(); j++){
                            temp[j] = command[j];
                        }
                        if (!T->Lookup(temp)) {
                            cout << "Sorry. This record is nonexistent." << endl;
                        }
                    }
                    
                }else{
                    cout << "Invalid input! Please enter command in correct format." << endl;
                    cout << "If you are confused with input format, please read \"README\" file or enter \"help\" for help." << endl;
                }
                
            }
        }else if (command == "help"){
            help();
        }else{
            cout << "Invalid input! The command line should stare with \"insert, lookup or help\"." << endl;
            cout << "If you are confused with input format, please read \"README\" file or enter \"help\" for help." << endl;
        }
    }
    delete T;
    return 0;
    
}
