// Calculates sum of all divisors of all integers up to 30000.

#include <iostream>
#include <cstdint>

uint64_t sum = 0;
uint64_t n = 30'000;

int main()
{
    for (uint64_t num = 1; num <= n; num++)
    {
        uint64_t sum_for_num = 0;

        for (uint64_t div = 1; div <= num; div++)
        {
            if (num % div == 0)
            {
                sum_for_num += div;
            }
        }

        sum += sum_for_num;
    }
}