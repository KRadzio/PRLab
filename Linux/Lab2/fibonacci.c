#include "fibonacci.h"
// 0 1
// 1 2 3 4 5 6
// 1 1 2 3 5 8

int fibonacci(int number)
{
    if(number < 1)
        return 0;
    else if(number == 1 || number == 2)
        return 1;
    else
    {
        int prev = 1;
        int current = 1;
        int next;
        for(int i = 2; i<number; i++)
        {
            next = current + prev;
            prev = current;
            current = next;
        }
        return next;
    }
}