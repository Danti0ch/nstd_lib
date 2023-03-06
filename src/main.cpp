#include <iostream>
#include "vector.hpp"
#include <iterator>
#include <vector>

int main(){

    nstd::vector<int> wtf(4, 1);
    nstd::vector<int> wtf2(5, 2);

    std::cout << wtf << "\n";
    std::cout << wtf2 << "\n";

    std::copy<nstd::vector<int>::iterator, nstd::vector<int>::iterator>(wtf.begin(), wtf.end(), wtf2.begin());
 
    std::cout << wtf << "\n";
    std::cout << wtf2 << "\n";
    return 0;
}
