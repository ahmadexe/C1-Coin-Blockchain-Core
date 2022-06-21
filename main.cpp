#include <iostream>
#include <bits/stdc++.h>
#include "rsa.h"
#include "ash.h"
#include <chrono>

using namespace std;

const int N = 1e5;
unsigned long long int coins[N];
vector<pair<double, int>> networkDelay[N];
double sumNetDelay = 0;
vector<bool> visitedNetwokTraversal(N, false);
vector<pair<int, int>> transaction[N];
// Queue, {sender's public key, { {ciphered message, vector}, {d, n}}}
vector < queue < pair < int, pair < pair < string, vector < unsigned long long int > >, pair < unsigned long long int, unsigned long long int > > > > > messagesVec[N];

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
    cout<<"--------------------------"<<endl;
    cout << "Private key of block: " << newBlock->index << endl;
    cout << "Public key of block: " << newBlock->publicKey << endl;
    cout<<"--------------------------"<<endl;
}

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
void hack()
{
    Block* temp = tail;
    temp->data = "Hacked";
}

void viewHashes()
{
    Block* temp = genesisBlock;
    while (temp)
    {
        cout<<"--------------------------"<<endl;
        cout << "Hash of block: " << temp->index << endl;
        cout << "Hash: " << temp->hash << endl;
        cout<<"--------------------------"<<endl;
        temp = temp->next;
    }
    cout<<endl;
}

void viewHashOfBlock(int privateKey)
{
    Block* temp = genesisBlock;
    while (temp && temp->index != privateKey)
    {
        temp = temp->next;
    }
    if (temp)
    {
        cout<<"--------------------------------"<<endl;
        cout<<temp->hash<<endl;
        cout<<"--------------------------------"<<endl;
    }
    else
    {
        cout<<endl;
        cout<<"Invalid private key"<<endl;
        cout<<endl;
    }
}

void printBlocks()
{
    Block *temp = genesisBlock;
    while (temp != NULL)
    {
        cout << "Block " << temp->publicKey << " ";
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
        transaction[sender].push_back(make_pair(r->publicKey, amount));
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
        cout << "History of " << temp->index << ", Current balance " << coins[temp->index] << endl;
        for (auto i : transaction[temp->index])
        {
            cout << "To " << i.first << ", Amount " << i.second << endl;
        }
    }
}

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
// index, delay, message, decryption key.

void sendMessage(int senderPrivateKey, int receiverPublicKey, string message)
{
    Block *s = genesisBlock;
    Block* r = genesisBlock;
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
    unsigned long long int n,phi,e,d, p, q;
    pair<unsigned long long int, unsigned long long int> primes = generatePrimes();
    p = primes.first;
    q = primes.second;
    n = p * q;
    phi = (p - 1) * (q - 1);
    e = generate_e(phi);
    d = generate_d(e, phi);

    pair<string, vector<unsigned long long int>> cipherPair = cipher(message, e, n);
    // Queue, {sender's public key, { {ciphered message, vector}, {d, n}}}
    queue < pair < int, pair < pair < string, vector<unsigned long long int>>, pair < unsigned long long int, unsigned long long int>>>> queue;
    queue.push({s->publicKey, {cipherPair, {d, n}}}); 
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
            pair<int, pair< pair < string, vector<unsigned long long int>>, pair < unsigned long long int, unsigned long long int>>> temp = i.front();
            i.pop();
            pair<string, vector<unsigned long long int>> cipherPair = temp.second.first;
            pair<unsigned long long int, unsigned long long int> dn = temp.second.second;
            string cipherText = cipherPair.first;
            string message = decipher(cipherPair.second, dn.first, dn.second);
            cout<<"----------------------------------------------------------"<<endl<<endl;
            cout << "From " << temp.first << ": Ciphered Text: "<<cipherText<<endl<<endl;
            cout << "From " << temp.first << ": Original Text: " << message << endl<<endl;
            cout<<"----------------------------------------------------------"<<endl;
        }
    }
    messagesVec->clear();
}

int main(int argc, char const *argv[])
{

    addGenesisBlock();
    // while (true)
    // {
    //     int choice;
    //     cout << "1. Add block: " << endl;
    //     cout << "2. send amound" << endl;
    //     cout << "3. View" << endl;
    //     cout << "4. Get hsitory" << endl;
    //     cout << "5. Send Message" << endl;
    //     cout << "6. check Messages" << endl;
    //     cout << "7. Hash" << endl;
    //     cout << "8. Validate" << endl;
    //     cout << "9. Exit" << endl;
    //     cin >> choice;
    //     string buffer;
    //     getline(cin, buffer);
    //     if (choice == 1)
    //     {
    //         string data;
    //         cout << "Enter the data: ";
    //         getline(cin, data);
    //         creatBlock(data);
    //     }
    //     else if (choice == 2)
    //     {
    //         int sender, receiver;
    //         unsigned long long int amount;
    //         cout << "Enter the sender's private key: ";
    //         cin >> sender;
    //         cout << "Enter the receiver's public key: ";
    //         cin >> receiver;
    //         cout << "Enter the amount: ";
    //         cin >> amount;
    //         transact(sender, receiver, amount);
    //     }
    //     else if (choice == 3)
    //     {
    //         printBlocks();
    //     }
    //     else if (choice == 4)
    //     {
    //         int privateKey;
    //         cout << "Enter the private key: ";
    //         cin >> privateKey;
    //         getHistory(privateKey);
    //     }
    //     else if (choice == 5)
    //     {
    //         int senderPrivateKey, receiverPublicKey;
    //         string message;
    //         cout << "Enter the sender's private key: ";
    //         cin >> senderPrivateKey;
    //         cout << "Enter the receiver's public key: ";
    //         cin >> receiverPublicKey;
    //         string buffer2;
    //         getline(cin, buffer2);
    //         cout << "Enter the message: ";
    //         getline(cin, message);
    //         sendMessage(senderPrivateKey, receiverPublicKey, message);
    //     }
    //     else if (choice == 6)
    //     {
    //         int privateKey;
    //         cout << "Enter the private key: ";
    //         cin >> privateKey;
    //         receiveMessage(privateKey);
    //     }
    //     else if (choice == 7)
    //     {
    //         viewHashes();
    //     }
    //     else if (choice == 8)
    //     {
    //         cout<< "Validation Status: " <<validateChain() << endl;
    //     }
    //     else if (choice == 9)
    //     {
    //         break;
    //     }
    // }

    while (true)
    {
        int choice;

        cout << "1. Add block." << endl;
        cout << "2. View All hashes." <<endl;
        cout << "3. View hash of a block." << endl;
        cout << "4. Validate blockchain." << endl;
        cout << "5. Make deposit." << endl;
        cout << "6. Get history of transactions on a block." << endl;
        cout << "7. View average time to find a block." << endl;
        cout << "8. Get total network since the formation of genesis block" << endl;
        cout << "9. Send a message." << endl;
        cout << "10. View all messages at a block" << endl;
        cout << "11. Alter a block" << endl;
        cout << "12. View all blocks" << endl;
        cout << "13. Exit " << endl;

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
            cout<<endl;
            int privateKey;
            cout << "Enter private key of the block: ";
            cin>>privateKey; 
            viewHashOfBlock(privateKey);
            cout<<endl;
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

        else if(choice == 5)
        {
            cout << endl;
        }

    }
    

    return 0;
}