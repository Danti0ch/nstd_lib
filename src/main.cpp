#include <iostream>
#include "vector.hpp"
#include "vector_bool.hpp"
#include <iterator>
#include <vector>
#include "allocator.hpp"

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

void test3() {
    
    nstd::vector<int> a;

    for(int i = 0; i < 20000; i++){
        a.push_back(i);
    }

    std::cout << "wtf\n";
    for(int i = 0; i < 20000; i++){
        std::cout << a[i] << " ";
    }

    std::cout << "qqq\n";
}

void test4() {

    nstd::vector<bool> v(12, false);
    std::cout << v.capacity() << "\n";
}

void test5() {

    nstd::vector<bool> v;
    
    v.push_back(true);
    v.push_back(false);
    v.push_back(true);
    v.push_back(false);

    for(nstd::vector<bool>::iterator it = v.begin(); it != v.end(); it++) {
        std::cout << (int)*it << " ";
    }
    std::cout << "\n";
    std::cout << v.capacity() << "\n";
}

int main(){
    //test1();
    test5();

    return 0;
}
