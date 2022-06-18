#include<iostream>
#include<bits/stdc++.h>
#include"rsa.h"
using namespace std;

// m^e mod n = c
// c^d mod n = m
// encryption key = (n,e)
// decryption key = (d)
// n = p*q
// phi = (p-1)*(q-1)

int main(int argc, char const *argv[])
{

    pair<unsigned long long int, unsigned long long int> primes = generatePrimes();
    unsigned long long int p = primes.first;
    unsigned long long int q = primes.second;
    cout<<"p = "<<p<<endl;
    cout<<"q = "<<q<<endl;
    unsigned long long int n = p*q;
    unsigned long long int phi = (p-1)*(q-1);
    unsigned long long int e = generate_e(phi);
    unsigned long long int d = generate_d(e, phi);
    string messgae;
    cout<<"Enter the message: ";
    getline(cin, messgae);
    cout<<"\n";
    pair<string, vector<unsigned long long int>> storeCiphers = cipher(messgae, e, n);
    cout<<"Encrypted message: ";
    cout<<storeCiphers.first;
    cout<<"\n";
    cout<<"Decrypted Message: ";
    cout<<decipher(storeCiphers.second, d, n);
    

    return 0;
}