#include "findMax.h"

int findMax(int *tab, int size)
{
    int max = -9999;
    for (int i = 0; i < size; i++)
    {
        if (tab[i] > max)
            max = tab[i];
    }
    return max;
}