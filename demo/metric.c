#include <stdio.h>

int main() {
    int a = 10, b = 5, c = 2;
    //lloc
    int result = a + b * c;
    //loc
    if (result > 15)
        result -= a / 2;
    else
        result += b - c;

    for (int i = 0; i < 3; i++)
        result += i * a;

    printf("%d\n", result);

    return 0;
}
