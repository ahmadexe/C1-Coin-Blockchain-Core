#include<iostream>
#include<bits/stdc++.h>
using namespace std;

// unsigned long long int n, e, d, p, q, phi;
// m^e mod n = c
// c^d mod n = m

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
    
    int p,q;
    srand((unsigned int)time(0));
    do
    {
        p = rand() % 10000+1;
        q = rand() % 10000+1;
    } while ((isPrime(p) && isPrime(q)) && (p != q));
    
    pair<unsigned long long int, unsigned long long int> primes = make_pair(p,q);
    return primes;
}