#include <bits/stdc++.h>
using namespace std;
 
int fib(int n) {
    std::cout << "\nValue: " << n << "\n";
    
    if (n <= 1)
        return n;
    
    return fib(n-1) + fib(n-2);
}
 
int main () {
    int n = 5;
    cout << fib(n);
    return 0;
}