#include <stdio.h>

int tribonacci(int n) {
    if (n == 0) {
        return 0;
    } else if (n == 1 || n == 2) {
        return 1;
    }

    int a = 0, b = 1, c = 1, temp;

    for (int i = 3; i <= n; i++) {
        temp = a + b + c;
        a = b;
        b = c;
        c = temp;
    }
    return c;
}

int main() {
    int n1 = 4;
    int n2 = 36;

    int result1 = tribonacci(n1);
    int result2 = tribonacci(n2);

    printf("Tribonacci(%d) = %d\n", n1, result1);
    printf("Tribonacci(%d) = %d\n", n2, result2);

    return 0;
}
