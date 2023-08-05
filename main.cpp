#include <iostream>
#include "dataset.cpp"

int add(int x, int y)
{
    return x + y;
}
int main()
{
    int (*ADD)(int, int) = add;
    ADD = add;
    std::cout << ADD(10, 20);
}

