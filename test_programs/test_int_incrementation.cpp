#include <iostream>
volatile int global_int_to_watch = 0;

int main()
{
    for (int i = 0; i < 5; ++i)
    {
        volatile int variable_that_reads_the_value = global_int_to_watch++;
    }
}