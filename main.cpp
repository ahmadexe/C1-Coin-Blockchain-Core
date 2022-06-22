#include <iostream>
#include <bits/stdc++.h>
#include "rsa.h"
#include "ash.h"
#include <chrono>

using namespace std;

// Declarations

const int N = 1e5;
double sumNetDelay = 0;

unsigned long long int coins[N];
vector<bool> visitedNetwokTraversal(N, false);
vector<pair<double, int>> networkDelay[N];
vector<pair<int, unsigned long long int>> transaction[N];
// Queue, {sender's public key, { {ciphered message, vector}, {d, n}}}
vector<queue<pair<int, pair<pair<string, vector<unsigned long long int>>, pair<unsigned long long int, unsigned long long int>>>>> messagesVec[N];


// Structure for every block in the blockchain.
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

// Converts block data into string so that it can be hashed later

string toString(Block *b)
{
    return (to_string(b->index) + " " + to_string(b->publicKey) + " " + b->previousHash + " " + b->data + " " + b->timestamp + " " + to_string(b->nonce));
}

// Hard coded method that adds the genesis block

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
    cout<<endl<<"------------------------------------"<<endl;
    cout << "Private key of genesis block: " << genesisBlock->index << endl;
    cout << "Public key of genesis block: " << genesisBlock->publicKey << endl;
    cout<<"------------------------------------"<<endl;
}

// Method to verify the nonce,
// Helps find a block

int verifyNonce(int nonceLastBlock)
{
    srand((unsigned)time(0));
    int random = (rand() % (nonceLastBlock + 2));
    return random;
}

// Method to create a block

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

    // The following loop finds a block. Notes the time and add that to delays array.

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

    // Outputs the public and private keys of blocks
    // The private keys start from 0, (Genisis block's)
    // The public keys start from 100, (Genisis block's)

    cout << "--------------------------" << endl;
    cout << "Private key of block: " << newBlock->index << endl;
    cout << "Public key of block: " << newBlock->publicKey << endl;
    cout << "--------------------------" << endl;
}

// Calculates and comapare the hashes to validate the chain.

int validateChain()
{
    Block *temp = genesisBlock;
    while (temp->next != NULL)
    {
        if (hashStr(toString(temp)) != temp->next->previousHash)
        {
            return 0;
        }
        if (temp->nextHash != hashStr(toString(temp->next)))
        {
            return 0;
        }
        temp = temp->next;
    }
    return 1;
}

// ! Only for testing purposes
// ! Tests if validation is working fine
void hack()
{
    Block *temp = tail;
    temp->data = "Hacked";
}

// Method to view all hashes

void viewHashes()
{
    Block *temp = genesisBlock;
    while (temp)
    {
        cout << "--------------------------" << endl;
        cout << "Hash of block: " << temp->index << endl;
        cout << "Hash: " << temp->hash << endl;
        cout << "--------------------------" << endl;
        temp = temp->next;
    }
    cout << endl;
}

// View hash of a particular block.

void viewHashOfBlock(int privateKey)
{
    Block *temp = genesisBlock;
    while (temp && temp->index != privateKey)
    {
        temp = temp->next;
    }
    if (temp)
    {
        cout << "--------------------------------" << endl;
        cout << temp->hash << endl;
        cout << "--------------------------------" << endl;
    }
    else
    {
        cout << endl;
        cout << "Invalid private key" << endl;
        cout << endl;
    }
}

// Prints the chain.

void printBlocks()
{
    Block *temp = genesisBlock;
    cout << endl;
    cout << "--------------------------------" << endl;
    while (temp != NULL)
    {
        cout << "Block " << temp->publicKey << " ";
        cout << coins[temp->index] << endl;
        temp = temp->next;
    }
    cout << "--------------------------------" << endl;
}

// Methods that obtains the block balance.

void makeBalance(int source)
{
    for (auto child : transaction[source])
    {
        coins[child.first] += child.second;
        coins[source] -= child.second;
    }
}

void generateCoinBalance()
{
    for (int i = 0; i < N; i++)
    {
        makeBalance(i);
    }
}

// Sends coin to the receiver, first time can only be called at genesis block.

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
        return;
    }
    if (!r)
    {
        cerr << "Invalid receiver's public key" << endl;
        return;
    }
    generateCoinBalance();
    if (coins[sender] < amount)
    {
        cerr << "Insufficient balance" << endl;
        return;
    }

    else
    {
        transaction[sender].push_back(make_pair(r->index, amount));
    }
    for (int i = 0; i < N; i++)
    {
        coins[i] = 0;
    }
    coins[0] = 1000;
}

// Gets history of all the transactions made from a block.

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
        cout << "History of " << temp->index << ", Current balance " << coins[temp->index] << endl;
        for (auto i : transaction[temp->index])
        {
            cout << "To " << (i.first + 100) << ", Amount " << i.second << endl;
        }
    }
}

// View the balance on a single block.

void viewBalanceOnBlock(int privateKey)
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
        cout << coins[privateKey];
    }
}

// Chains length.

int getChainLength()
{
    int length = 0;
    Block *temp = genesisBlock;
    while (temp)
    {
        length++;
        temp = temp->next;
    }
    return length;
}

// Get the delay in findind blocks.
// (Time taken in searching blocks.)

double getNetworkDelay(int genesisIndex)
{
    if (visitedNetwokTraversal[genesisIndex])
    {
        return 0.0;
    }
    visitedNetwokTraversal[genesisIndex] = true;
    if (networkDelay[genesisIndex].size() == 0)
    {
        return 0.0;
    }
    sumNetDelay += networkDelay[genesisIndex][networkDelay[genesisIndex].size() - 1].first;
    for (auto i : networkDelay[genesisIndex])
    {
        getNetworkDelay(i.second);
    };
    return sumNetDelay;
}

// index, delay, message, decryption key. (Format messagesVec)
// Send messages to receiver.

void sendMessage(int senderPrivateKey, int receiverPublicKey, string message)
{
    Block *s = genesisBlock;
    Block *r = genesisBlock;
    while (s && s->index != senderPrivateKey)
    {
        s = s->next;
    }
    while (r && r->publicKey != receiverPublicKey)
    {
        r = r->next;
    }
    if (!s)
    {
        cerr << "Invalid private key" << endl;
        return;
    }
    if (!r)
    {
        cerr << "Invalid receiver's public key" << endl;
        return;
    }
    if (s == r)
    {
        cerr << "Sender and receiver are the same" << endl;
        return;
    }

    // RSA Variables.
    unsigned long long int n, phi, e, d, p, q;
    // Method to generate primes of the power 1e5
    pair<unsigned long long int, unsigned long long int> primes = generatePrimes();
    p = primes.first;
    q = primes.second;
    n = p * q;
    phi = (p - 1) * (q - 1);
    // generates encryption key.
    e = generate_e(phi);
    // generates decryption key.
    d = generate_d(e, phi);

    // Encrypts message.
    pair<string, vector<unsigned long long int>> cipherPair = cipher(message, e, n);
    // Queue, {sender's public key, { {ciphered message, vector}, {d, n}}}
    queue<pair<int, pair<pair<string, vector<unsigned long long int>>, pair<unsigned long long int, unsigned long long int>>>> queue;
    // Pushes into the messaging queue.
    queue.push({s->publicKey, {cipherPair, {d, n}}});
    // Pushes the queue to the message vector.
    messagesVec[r->index].push_back(queue);
}

void receiveMessage(int receiverPrivateKey)
{
    Block *r = genesisBlock;
    while (r && r->index != receiverPrivateKey)
    {
        r = r->next;
    }
    if (!r)
    {
        cerr << "Invalid private key" << endl;
        return;
    }
    if (messagesVec[r->index].size() == 0)
    {
        cerr << "No messages" << endl;
        return;
    }
    if (messagesVec[r->index][messagesVec[r->index].size() - 1].size() == 0)
    {
        cerr << "No messages" << endl;
        return;
    }
    for (auto i : messagesVec[r->index])
    {
        while (!i.empty())
        {
            pair<int, pair<pair<string, vector<unsigned long long int>>, pair<unsigned long long int, unsigned long long int>>> temp = i.front();
            i.pop();
            pair<string, vector<unsigned long long int>> cipherPair = temp.second.first;
            pair<unsigned long long int, unsigned long long int> dn = temp.second.second;
            string cipherText = cipherPair.first;
            string message = decipher(cipherPair.second, dn.first, dn.second);
            cout << "----------------------------------------------------------" << endl
                 << endl;
            cout << "From " << temp.first << ": Ciphered Text: " << cipherText << endl
                 << endl;
            cout << "From " << temp.first << ": Original Text: " << message << endl
                 << endl;
            cout << "----------------------------------------------------------" << endl;
        }
    }
    messagesVec->clear();
}

int main(int argc, char const *argv[])
{

    addGenesisBlock();

    while (true)
    {
        int choice;

        cout << "1. Add block." << endl;
        cout << "2. View All hashes." << endl;
        cout << "3. View hash of a block." << endl;
        cout << "4. Validate blockchain." << endl;
        cout << "5. Make deposit." << endl;
        cout << "6. View coins in a block" << endl;
        cout << "7. Get history of transactions on a block." << endl;
        cout << "8. View average time to find a block." << endl;
        cout << "9. Get total network since the formation of genesis block" << endl;
        cout << "10. Send a message." << endl;
        cout << "11. View all messages at a block" << endl;
        cout << "12. Alter a block" << endl;
        cout << "13. View all blocks" << endl;
        cout << "14. Exit " << endl;

        cin >> choice;
        string buffer;
        getline(cin, buffer);

        if (choice == 1)
        {
            string data;
            cout << "Enter a unique data: ";
            getline(cin, data);
            creatBlock(data);
        }

        else if (choice == 2)
        {
            cout << endl;
            viewHashes();
            cout << endl;
        }

        else if (choice == 3)
        {
            cout << endl;
            int privateKey;
            cout << "Enter private key of the block: ";
            cin >> privateKey;
            viewHashOfBlock(privateKey);
            cout << endl;
        }

        else if (choice == 4)
        {
            cout << endl;
            int validateVal;
            validateVal = validateChain();
            if (validateVal)
            {
                cout << "The chain is valid" << endl;
            }
            else
            {
                cout << "The chain has been corrupted" << endl;
            }
            cout << endl;
        }

        else if (choice == 5)
        {
            cout << endl;
            int sender, receiver;
            unsigned long long int amount;
            cout << "Enter the sender's private key: ";
            cin >> sender;
            cout << "Enter the receiver's public key: ";
            cin >> receiver;
            cout << "Enter the amount: ";
            cin >> amount;
            transact(sender, receiver, amount);
            cout << endl;
        }

        else if (choice == 6)
        {
            cout << endl;
            int privateKey;
            cout << "Enter the sender's private key: ";
            cin >> privateKey;
            generateCoinBalance();
            viewBalanceOnBlock(privateKey);
            for (int i = 0; i < N; i++)
            {
                coins[i] = 0;
            }
            coins[0] = 1000;
            cout << endl;
        }

        else if (choice == 7)
        {
            cout << endl;
            int privateKey;
            cout << "Enter the sender's private key: ";
            cin >> privateKey;
            generateCoinBalance();
            getHistory(privateKey);
            for (int i = 0; i < N; i++)
            {
                coins[i] = 0;
            }
            coins[0] = 1000;
            cout << endl;
        }

        else if (choice == 8)
        {
            cout << endl;
            cout << "Average time to find a block: " << (getNetworkDelay(0) / getChainLength()) << "s" << endl;
            cout << endl;
            // visitedNetwokTraversal.clear();
            for (int i = 0; i < N; i++)
            {
                visitedNetwokTraversal[i] = false;
            }
        }

        else if (choice == 9)
        {
            cout << endl;
            cout << "Total network since the formation of genesis block: " << getNetworkDelay(0) << "s" << endl;
            cout << endl;
            for (int i = 0; i < N; i++)
            {
                visitedNetwokTraversal[i] = false;
            }
        }

        else if (choice == 10)
        {
            cout << endl;
            int senderPrivateKey, receiverPublicKey;
            string message;
            cout << "Enter the sender's private key: ";
            cin >> senderPrivateKey;
            cout << "Enter the receiver's public key: ";
            cin >> receiverPublicKey;
            string buffer2;
            getline(cin, buffer2);
            cout << "Enter the message: ";
            getline(cin, message);
            sendMessage(senderPrivateKey, receiverPublicKey, message);
            cout << endl;
        }

        else if (choice == 11)
        {
            cout << endl;
            int privateKey;
            cout << "Enter the private key: ";
            cin >> privateKey;
            receiveMessage(privateKey);
            cout << endl;
        }

        else if (choice == 12)
        {
            cout << endl;
            hack();
            cout << "Blocks altered" << endl;
            cout << endl;
        }

        else if (choice == 13)
        {
            generateCoinBalance();
            printBlocks();
            for (int i = 0; i < N; i++)
            {
                coins[i] = 0;
            }
            coins[0] = 1000;
        }

        else if (choice == 14)
        {
            break;
        }

        else
        {
            cout << endl;
            cout << "Invalid choice" << endl;
            cout << endl;
        }
    }

    return 0;
}