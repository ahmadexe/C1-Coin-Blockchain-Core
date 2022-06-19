#include <iostream>
#include <bits/stdc++.h>
#include "rsa.h"
#include "ash.h"
#include <chrono>

using namespace std;

const int N = 1e5;
unsigned long long int coins[N];
vector<pair<double, int>> networkDelay[N];
vector<pair<int, int>> transaction[N];
// index, delay, message, decryption key.
vector<pair<int, pair<int, pair<string, unsigned long long int>>>> message[N];

struct Block
{
    int index;
    int publicKey;
    unsigned long long int COINS;
    string hash;
    string previousHash;
    string nextHash;
    string data;
    string timestamp;
    int nonce;
    Block *next;
    Block *previous;
};

Block *genesisBlock = NULL;
Block *tail = NULL;

string toString(Block *b)
{
    return (to_string(b->index) + " " + to_string(b->publicKey) + " " + b->previousHash + " " + b->data + " " + b->timestamp + " " + to_string(b->nonce));
}

void addGenesisBlock()
{
    genesisBlock = new Block;
    genesisBlock->index = 0;
    genesisBlock->publicKey = genesisBlock->index + 100;
    genesisBlock->COINS = 1000;
    coins[genesisBlock->index] = genesisBlock->COINS;
    genesisBlock->previousHash = "0";
    genesisBlock->data = "Genisis Block";
    genesisBlock->timestamp = to_string(chrono::system_clock::now().time_since_epoch().count());
    genesisBlock->nonce = 0;
    genesisBlock->next = NULL;
    genesisBlock->previous = NULL;
    genesisBlock->hash = hashStr(toString(genesisBlock));
    tail = genesisBlock;
    cout << "Private key of genesis block: " << genesisBlock->index << endl;
    cout << "Public key of genesis block: " << genesisBlock->publicKey << endl;
}

int verifyNonce(int nonceLastBlock)
{
    srand((unsigned)time(0));
    int random = (rand() % (nonceLastBlock + 2));
    return random;
}

void creatBlock(string data)
{
    Block *newBlock = new Block;
    newBlock->COINS = 0;
    newBlock->index = tail->index + 1;
    newBlock->publicKey = newBlock->index + 100;
    newBlock->previousHash = tail->hash;
    newBlock->data = data;
    newBlock->timestamp = to_string(chrono::system_clock::now().time_since_epoch().count());
    newBlock->nonce = tail->nonce + 1;
    auto start = chrono::steady_clock::now();

    int checkNonce = verifyNonce(tail->nonce);
    do
    {
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
    cout << "Private key of block: " << newBlock->index << endl;
    cout << "Public key of block: " << newBlock->publicKey << endl;
}

void printBlocks()
{
    Block *temp = genesisBlock;
    while (temp != NULL)
    {
        cout << "Block " << temp->index << " ";
        cout << coins[temp->index] << endl;
        temp = temp->next;
    }
}

void transact(int sender, int receiver, unsigned long long int amount)
{
    Block *s = genesisBlock;
    Block *r = genesisBlock;
    while (s && s->index != sender)
    {
        s = s->next;
    }
    while (r && r->publicKey != receiver)
    {
        r = r->next;
    }
    if (!s)
    {
        cerr << "Invalid private key" << endl;
    }
    if (!r)
    {
        cerr << "Invalid receiver's public key" << endl;
    }
    if (coins[sender] < amount)
    {
        cerr << "Insufficient balance" << endl;
    }
    else
    {
        coins[r->index] += amount;
        coins[sender] -= amount;
        transaction[sender].push_back(make_pair(r->index, amount));
    }
}

void getHistory(int privateKey)
{
    Block *temp = genesisBlock;
    while (temp && temp->index != privateKey)
    {
        temp = temp->next;
    }
    if (!temp)
    {
        cerr << "Invalid private key" << endl;
    }
    else
    {
        cout << "History of " << temp->index << " " << coins[temp->index] << endl;
        for (auto i : transaction[temp->index])
        {
            cout << "To " << i.first << " " << i.second << endl;
        }
    }
}

int main(int argc, char const *argv[])
{

    addGenesisBlock();
    while (true)
    {
        int choice;
        cout << "1. Add block: " << endl;
        cout << "2. send amound" << endl;
        cout << "3. View" << endl;
        cout << "4. exit" << endl;
        cin >> choice;
        string buffer;
        getline(cin, buffer);
        if (choice == 1)
        {
            string data;
            cout << "Enter the data: ";
            getline(cin, data);
            creatBlock(data);
        }
        else if (choice == 2)
        {
            int sender, receiver;
            unsigned long long int amount;
            cout << "Enter the sender's private key: ";
            cin >> sender;
            cout << "Enter the receiver's public key: ";
            cin >> receiver;
            cout << "Enter the amount: ";
            cin >> amount;
            transact(sender, receiver, amount);
        }
        else if (choice == 3)
        {
            printBlocks();
        }
        else if (choice == 4)
        {
            break;
        }
    }

    return 0;
}