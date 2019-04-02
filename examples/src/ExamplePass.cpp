#include <iostream>
#include "EasyCL/EasyCL.hpp"

void foo(int a){
    std::cout << a << std::endl;
}

void foo(const int* a){
    std::cout << a[0] << std::endl;
    std::cout << a[1] << std::endl;
    std::cout << a[2] << std::endl;
}

int main()
{
    ecl::Variable<int> a = 5;

    int B[] = {1, 2, 3};
    ecl::Array<int> b(B, 3);

    foo(a);
    foo(b);

    return 0;
}