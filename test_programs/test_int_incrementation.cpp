#include <iostream>
volatile int global_int_to_watch = 0;

int main()
{
    int other_1 = global_int_to_watch;
    global_int_to_watch = 1;
    int other_2 = global_int_to_watch;
    //for (int i = 0; i < 5; ++i)
    //{
    //    volatile int variable_that_reads_the_value = global_int_to_watch++;
    //}
}