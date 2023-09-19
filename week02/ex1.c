#include <stdio.h>
#include <limits.h>
#include <float.h>

int main() {
    int integer = INT_MAX;
    unsigned short int unsignedShortInteger = USHRT_MAX;
    long int signedLongInt = LONG_MAX;
    float floatVar = FLT_MAX;
    double doubleVar = DBL_MAX;

    printf("Size of integer: %lu bytes\n", sizeof(integer));
    printf("Value of integer: %d\n", integer);

    printf("Size of unsigned short integer: %lu bytes\n", sizeof(unsignedShortInteger));
    printf("Value of unsigned short integer: %u\n", unsignedShortInteger);

    printf("Size of signed long int: %lu bytes\n", sizeof(signedLongInt));
    printf("Value of signed long int: %ld\n", signedLongInt);

    printf("Size of float: %lu bytes\n", sizeof(floatVar));
    printf("Value of float: %f\n", floatVar);

    printf("Size of double: %lu bytes\n", sizeof(doubleVar));
    printf("Value of double: %lf\n", doubleVar);

    return 0;
}
