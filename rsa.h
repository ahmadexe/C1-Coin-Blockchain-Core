#include<iostream>
#include<bits/stdc++.h>
using namespace std;

// unsigned long long int n, e, d, p, q, phi;
// m^e mod n = c
// c^d mod n = m

// m^e mod n = c
// c^d mod n = m
// encryption key = (n,e)
// decryption key = (d)
// n = p*q
// phi = (p-1)*(q-1)

/*
* Function to generate primes
* @return pair<unsigned long long int, unsigned long long int>
*/
int isPrime(unsigned long long int n)
{
    if(n==2)
        return 1;
    if(n==1 || n%2==0)
        return 0;
    for(int i=3; i*i<=n; i+=2)
        if(n%i==0)
            return 0;
    return 1;
}

pair<unsigned long long int, unsigned long long int> generatePrimes() {
    
    unsigned long long int p,q;
    srand((unsigned int)time(0));
    do
    {
        p = rand() % 10000+1;
        q = rand() % 10000+1;
    } while (!isPrime(p) || !isPrime(q) || p==q);
    
    pair<unsigned long long int, unsigned long long int> primes = {p,q};
    return primes;
}

/*
* Function to generate e
* @return unsigned long long int
*/
unsigned long long int gcd(unsigned long long int a, unsigned long long int b)
{
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

unsigned long long int generate_e(unsigned long long int phi)
{
    unsigned long long int e = 2;
    int gcdv = 0;
    while (gcdv != 1)
    {
        e++;
        gcdv = gcd(e, phi);
    }
    return e;
}

/*
* Function to generate d
* @return unsigned long long int
*/
unsigned long long int generate_d(unsigned long long int e,unsigned long long int phi) {
    unsigned long long int d = 1;
    while (1) {
        unsigned long long int x = (e * d) % phi;
        if (x == 1)
            return d;
        d++;
    }
}


/*
* Function to cipher
* Uses modular arithmetic ecponential rule to calculate values
* @return pair<string, vector<unsigned long long int>>
*/
unsigned long long int modArth(long long int base, long long int exponent, long long int mod) {
    if (mod == 1)
        return 0;
    int c = 1;
    for (int i = 0; i < exponent; i++) {
        c = (c * base) % mod;
    }
    return c;
}

pair<string, vector<unsigned long long int>> cipher(string message, unsigned long long int e, unsigned long long int n) {
    vector<unsigned long long int> cipherVector; 
    string ciphertext = "";
    for (int i = 0; i < message.length(); i++) {
        int c = message[i];
        unsigned long long int ciphr = modArth(c, e, n);
        cipherVector.push_back(ciphr);
        ciphertext += to_string(ciphr);
    }
    srand(time(NULL));
    random_shuffle(ciphertext.begin(), ciphertext.end());
    return {ciphertext, cipherVector};
}

/*
* Function to decipher
* Uses modular arithmetic ecponential rule to calculate values
* @return string
*/
string decipher(vector<unsigned long long int> cipherVector, unsigned long long int d, unsigned long long int n) {
    string message = "";
    for (int i = 0; i < cipherVector.size(); i++) {
        unsigned long long int m = modArth(cipherVector[i], d, n);
        char append = (char)m;
        message += append;
    }
    return message;
}