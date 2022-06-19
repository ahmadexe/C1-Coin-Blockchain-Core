#include<iostream>
#include<bits/stdc++.h>
#include"rsa.h"
#include"ash.h"
#include<chrono>

using namespace std;

const int N = 1e5;
vector<pair<double,int>> networkDelay[N];
vector<pair<int,int>> transaction[N];
// index, delay, message, decryption key.
vector<pair<int, pair<int, pair<string, unsigned long long int>>>> message[N];

struct Block
{
    int index;
    unsigned long long int COINS;
    string hash;
    string previousHash;
    string nextHash;
    string data;
    string timestamp;
    int nonce;
    Block* next;
    Block* previous;
};

Block* genesisBlock = NULL;
Block* tail = NULL;

string toString(Block* b)
{
    return (to_string(b->index) + " " + b->previousHash +" " + b->data + " " + b->timestamp + " " + to_string(b->nonce));
}

void addGenesisBlock()
{
    genesisBlock = new Block;
    genesisBlock->index = 0;
    genesisBlock->COINS = 1000;
    genesisBlock->previousHash = "0";
    genesisBlock->data = "Genisis Block";
    genesisBlock->timestamp = to_string(chrono::system_clock::now().time_since_epoch().count());
    genesisBlock->nonce = 0;
    genesisBlock->next = NULL;
    genesisBlock->previous = NULL;
    genesisBlock->hash = hashStr(toString(genesisBlock));
    tail = genesisBlock;
}

int verifyNonce(int nonceLastBlock)
{
    srand((unsigned) time(0));
    int random = (rand() % (nonceLastBlock+2));
    return random;
}

void creatBlock(string data) 
{
    Block* newBlock = new Block;
    newBlock->index = tail->index + 1;
    newBlock->previousHash = tail->hash;
    newBlock->data = data;
    newBlock->timestamp = to_string(chrono::system_clock::now().time_since_epoch().count());
    newBlock->nonce = tail->nonce + 1;
    auto start = chrono::steady_clock::now();

    int checkNonce = verifyNonce(tail->nonce);
    do{
        checkNonce = verifyNonce(tail->nonce);
    } while (checkNonce != newBlock->nonce);    
    auto end = chrono::steady_clock::now();
    double elapsedTime = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    elapsedTime = elapsedTime / 1e9;
    
    networkDelay[tail->index].push_back(make_pair(elapsedTime, newBlock->index));
    
    newBlock->next = NULL;
    newBlock->previous = tail;
    newBlock->hash = hashStr(toString(newBlock));
    tail->next = newBlock;
    tail->nextHash = newBlock->hash;
    tail = newBlock;
}

void printBlocks()
{
    Block* temp = genesisBlock;
    while(temp != NULL)
    {
        cout<<"Block "<<temp->index<<" ";
        temp = temp->next;
    }
}



int main(int argc, char const *argv[])
{

    addGenesisBlock();
    while (true)
    {   int choice;
        cout<<"1. Add block: "<<endl;
        cout<<"2. Print blocks"<<endl;
        cout<<"3. Exit"<<endl;
        cin>>choice;
        string buffer;
        getline(cin, buffer);
        if (choice == 1)
        {
            string data;
            cout<<"Enter the data: ";
            getline(cin, data);
            creatBlock(data);
        }
        else if (choice == 2)
        {
            printBlocks();
        }
        else if (choice == 3)
        {
            break;
        }        
    }
    
    return 0;
}