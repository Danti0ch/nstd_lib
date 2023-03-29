#include <iostream>
#include "function.hpp"

int func2(const int * x, int y) {
	return (*x) + y;
}

int func1() {
	return 0;
}

void test1() {
    typedef nstd::function<int (const int * , int)> int_function_with_two_args_t;
	int_function_with_two_args_t f2(func2);
	int x = 10;
	std::cout << "calling function with signature int (const int * , int): " <<  f2(&x, 20) << "\n";
}

void test2() {
    typedef nstd::function<int (void)> int_function_t;
	int_function_t f1(func1);
	std::cout << "calling function with signature int (void): " <<  f1() << "\n";

	int_function_t f2;
	f2 = f1;
	std::cout << "calling function after assignment operator with signature int (void): " <<  f2() << "\n";

	int_function_t f3(f2);
	std::cout << "calling function after copying ctor with signature int (void): " <<  f3() << "\n";
}

struct Foo {
	int smth(int x) {
		return x + 1;
	}
};

void test3() {
	typedef nstd::function<int (Foo, int)> member_function_t;
	member_function_t f1 = &Foo::smth;

	Foo foo;
	std::cout << "calling member function with signature int (int): " <<  f1(foo, 5) << "\n";
}

int main() {

    test1();
    test2();
    test3();

    return 0;
}
