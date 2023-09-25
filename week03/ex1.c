#include <stdio.h>
#include <stdlib.h>

int const_tri(int *p, int n) {
    if (n == 0) {
        return 0;
    } else if (n == 1 || n == 2) {
        return *p;
    } else if (n == 3) {
        return *(p+2);
    }
    int temp;

    for (int i = 4; i <= n; i++) {
        temp = *p + *(p + 1) + *(p + 2);
        *p = *(p + 1);
        *(p + 1) = *(p + 2);
        *(p + 2) = temp;
    }

    return *(p + 2);
}


int main() {
    const int *q = &(const int) {1};
    int *const p = malloc(3 * sizeof(int));

    *p = *q;
    *(p + 1) = *p;
    *(p + 2) = *(p + 1) + *p;

    printf("Memory addresses: 0)%p 1)%p 2)%p\n", p, p + 1, p + 2);

    int address1 = (int) (p);
    int address2 = (int) (p + 1);
    int address3 = (int) (p + 2);

    if (address2 == address1 + sizeof(int) && address3 == address2 + sizeof(int)) {
        printf("The memory cells are continuous.\n");
    } else {
        printf("The memory cells are not continuous.\n");
    }

    printf("Tribonacci(4) = %d\n", const_tri(p, 5));

    free(p);

    return 0;

}
