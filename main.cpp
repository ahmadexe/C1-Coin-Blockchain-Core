#include<iostream>
#include<bits/stdc++.h>
#include"rsa.h"
#include"ash.h"
#include<chrono>

using namespace std;

const int N = 1e5;
vector<pair<double,int>> networkDelay[N];
vector<pair<int,int>> transaction[N];
vector<pair<string, int>> message[N];

struct Block
{
    int index;
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
    srand(time(0));
    int random = rand() % nonceLastBlock+2;
    return random;
}

void creatBlock() 
{

}

int main(int argc, char const *argv[])
{

    


    return 0;
}