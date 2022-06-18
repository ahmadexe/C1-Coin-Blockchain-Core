// This dumb program is a simple algorithm to hash a string.
// This hasn't been tested on enough inputs and will surely generate collisions but works for now.

#include<iostream>
#include<bits/stdc++.h>
#include"ash.h"

using namespace std;


int main(int argc, char const *argv[])
{

    string str, str2;
    cout<<"Enter the message: ";
    getline(cin, str);
    cout<<"\n";
    cout<<"Enter the message: ";
    getline(cin, str2);
    cout<<"\n";

    string x = hashStr(str);
    string y = hashStr(str2);
    cout<<(x == y);

    return 0;
}