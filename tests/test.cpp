#include <iostream>
volatile int global_variable;

int main()
{
    for(; global_variable < 1000'000'000; global_variable++);
}