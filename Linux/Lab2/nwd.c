#include "nwd.h"

int nwd(int a, int b)
{
    if (a <= 0 || b <= 0)
        return 0;

    int temp;
    while (b != 0)
    {
        temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}