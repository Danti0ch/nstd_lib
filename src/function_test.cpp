#include <iostream>
#include "function.hpp"

int func2(const int * x, int y) {
	return (*x) + y;
}

int main() {

	typedef nstd::function<int (const int * , int)> int_function_with_two_args_t;
	int_function_with_two_args_t f2(func2);
	int x = 10;
	std::cout << "calling function with signature int (const int * , int): " <<  f2(&x, 20) << "\n";

    return 0;
}
