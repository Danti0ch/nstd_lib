#include <iostream>
#include "vector.hpp"
#include <iterator>
#include <vector>

void test1() {
    nstd::vector<int> a(4, 1);
    for(int i = 0; i < 4; i++)
    { a[i] = i + 1; }

    nstd::vector<int> b(5, 2);
    for(int i = 0; i < 5; i++)
    {b[i] = 100 * (i + 1); }

    std::cout << "a: " << a << "\n";
    std::cout << "b: " << b << "\n";

    std::copy<nstd::vector<int>::iterator, nstd::vector<int>::reverse_iterator>(a.begin(), a.end(), b.begin());
    
    std::cout << "a: " << a << "\n";
    std::cout << "b: " << b << "\n"; 

    std::vector<std::vector<int>> v;
}

void test2() {
    nstd::vector<int> a(4, 1);
    std::cout << "a: " << a << "\n";

    int base = 1;
    for(nstd::vector<int>::reverse_iterator riter = a.rbegin(); riter != a.rend(); riter++) {
        *riter = base <<= 1;
    }

    std::cout << "a: " << a << "\n";
}

int main(){

    //test1();
    test2();

    return 0;
}
