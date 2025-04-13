#include "nww.h"

int nww(int a, int b)
{
    if (a <= 0 || b <= 0)
        return 0;
    else
        return a / nwd(a, b) * b;
}