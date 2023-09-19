#include <stdio.h>
#include <math.h>

// Function to calculate the number of digits in a number
int countDigits(long long num, int base) {
    return (int)(log(num) / log(base)) + 1;
}

// Function to convert a number from one numeral system to another
void convert(long long number, int s, int t) {
    // ((s > 10 || s < 2) || (t > 10 || t < 2))
    if ((s > 10 || s < 2) || (t > 10 || t < 2)) {
        printf("cannot convert!\n");
        return;
    }

    long long result = 0;
    long long base = 1;

    while (number > 0) {
        int digit = number % 10;
        if (digit >= s) {
            printf("cannot convert!\n");
            return;
        }
        result += digit * base;
        base *= s;
        number /= 10;
    }

    int numDigits = countDigits(result, t);

    int digits[numDigits];
    for (int i = 0; i < numDigits; i++) {
        digits[i] = result % t;
        result /= t;
    }

    printf("Converted: \"");
    for (int i = numDigits - 1; i >= 0; i--) {
        putchar('0' + digits[i]);
    }
    printf("\"\n");
}

int main() {
    long long number;
    int source, target;

    printf("Enter a non-negative number: ");
    if (scanf("%lld", &number) != 1 || number < 0) {
        printf("Error: Invalid input!\n");
        return 1;
    }

    printf("Enter the source numeral system (2-10): ");
    if (scanf("%d", &source) != 1) {
        printf("Error: Invalid input!\n");
        return 1;
    }

    printf("Enter the target numeral system (2-10): ");
    if (scanf("%d", &target) != 1) {
        printf("Error: Invalid input!\n");
        return 1;
    }

    convert(number, source, target);

    return 0;
}
