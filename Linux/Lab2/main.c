#include <stdio.h>

#include "factorial.h"
#include "fibonacci.h"
#include "findMax.h"
#include "isPrime.h"
#include "nwd.h"
#include "nww.h"

int main()
{
    printf("%d %d %d %d %d \n", factorial(-1), factorial(0), factorial(1), factorial(3),factorial(4));
    printf("%d %d %d %d %d \n", fibonacci(-1), fibonacci(1), fibonacci(2), fibonacci(3), fibonacci(5));
    printf("%d %d %d %d %d \n", isPrime(0), isPrime(2), isPrime(5), isPrime(4), isPrime(6));
    int tab[] = {1,5,7,89,2,1000,5,42,9999};
    printf("%d \n", findMax(tab,9));
    printf("%d %d %d %d %d \n", nwd(0,1), nwd(8,16), nwd(36,24), nwd(64,80), nwd(27,102));
    printf("%d %d %d %d %d \n", nww(0,1), nww(2,10), nww(21,37), nww(26,72), nww(45,30));
    return 0;
}