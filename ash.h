// This dumb program is a simple algorithm to hash a string.
// This hasn't been tested on enough inputs and will surely generate collisions but works for now.

#include<iostream>
#include<bits/stdc++.h>
using namespace std;

const int a = 31;
const int b = 15;

string hashStr(string str)
{
    string hash = "";
    int i = 0;
    int j = 1;
    while(j < str.length())
    {
        int x = int(str[i]);
        int y = int(str[j]);
        x = x % a;
        y = y % b;
        int z = x << y;
        hash += to_string(z);
        i++;
        j++;
    }
    string hashS = "";
    for(int i = 0; i < hash.length(); i++)
    {
        int n = hash[i] - 48;
        hashS += char(n);
    }
    return hashS;
}