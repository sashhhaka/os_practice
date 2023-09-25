#include <stdio.h>
#include <math.h>

struct Point {
    double x;
    double y;
};

double distance(struct Point p1, struct Point p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

double area(struct Point p1, struct Point p2, struct Point p3) {
    return (p1.x * p2.y - p2.x * p1.y + p2.x * p3.y - p3.x * p2.y + p3.x * p1.y - p1.x * p3.y)/2;
}

int main() {
    struct Point A = {2.5, 6};
    struct Point B = {1, 2.2};
    struct Point C = {10, 6};
    double AB = distance(A, B);
    double areaABC = area(A, B, C);
    printf("AB = %lf\n", AB);
    printf("areaABC = %lf\n", areaABC);

    return 0;
}
