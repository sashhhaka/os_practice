#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>

void* aggregate(void* base, size_t size, int n, void* initialValue, void* (*opr)(const void*, const void*)) {
    char* basePtr = (char*)base;
    void* result = initialValue;
    void* pre_result;

    for (int i = 0; i < n; i++) {
        pre_result = result;
        result = opr(result, basePtr);
        basePtr += size;
        if (i > 0) {
            free(pre_result);
        }
    }

    return result;
}

void* add_int(const void* a, const void* b) {
    int* result = (int*)malloc(sizeof(int)); // Allocate memory for the result
    *result = *((int*)a) + *((int*)b);
    return result;
}

void* add_double(const void* a, const void* b) {
    double* result = (double*)malloc(sizeof(double)); // Allocate memory for the result
    *result = *((double*)a) + *((double*)b);
    return result;
}

void* mul_int(const void* a, const void* b) {
    int* result = (int*)malloc(sizeof(int));
    *result = *((int*)a) * *((int*)b);
    return result;
}

void* mul_double(const void* a, const void* b) {
    double* result = (double*)malloc(sizeof(double)); // Allocate memory for the result
    *result = *((double*)a) * *((double*)b);
    return result;
}

void* max_int(const void* a, const void* b) {
    int* result = (int*)malloc(sizeof(int));
    *result = *((int*)a) > *((int*)b) ? *((int*)a) : *((int*)b);
    return result;
}

void* max_double(const void* a, const void* b) {
    double* result = (double*)malloc(sizeof(double)); // Allocate memory for the result
    *result = *((double*)a) > *((double*)b) ? *((double*)a) : *((double*)b);
    return result;
}


int main() {
    int intArray[] = {1, 2, 3, 4, 5};
    double doubleArray[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    int n = 5;

    int intInitialValueAdd = 0;
    int intInitialValueMul = 1;
    int intInitialValueMax = INT_MIN;
    double doubleInitialValueAdd = 0.0;
    double doubleInitialValueMul = 1.0;
    double doubleInitialValueMax = -DBL_MAX;

    // Aggregate using addition for int
    int* intSum = (int*)aggregate(intArray, sizeof(int), n, &intInitialValueAdd, add_int);
    printf("Sum of integers: %d\n", *intSum);
    int* intMul = (int*)aggregate(intArray, sizeof(int), n, &intInitialValueMul, mul_int);
    printf("Multiplication of integers: %d\n", *intMul);
    int* intMax = (int*)aggregate(intArray, sizeof(int), n, &intInitialValueMax, max_int);
    printf("Max of integers: %d\n\n", *intMax);

    // Aggregate using addition for double
    double* doubleSum = (double*)aggregate(doubleArray, sizeof(double), n, &doubleInitialValueAdd,
                                           add_double);
    printf("Sum of doubles: %.2lf\n", *doubleSum);
    double* doubleMul = (double*)aggregate(doubleArray, sizeof(double), n, &doubleInitialValueMul,
                                           mul_double);
    printf("Multiplication of doubles: %.2lf\n", *doubleMul);
    double* doubleMax = (double*)aggregate(doubleArray, sizeof(double), n, &doubleInitialValueMax,
                                           max_double);
    printf("Max of doubles: %.2lf\n", *doubleMax);

    // Free allocated memory
    free(intSum);
    free(doubleSum);

    return 0;
}
