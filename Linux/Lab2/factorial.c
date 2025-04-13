#include "factorial.h"

int factorial(int number)
{
    if (number < 0)
        return 0;
    else if (number == 0 || number == 1)
        return 1;
    else
    {
        int f = 1;
        for(int i=2; i<= number; i++)
            f*=i;
        return f;
    }
}