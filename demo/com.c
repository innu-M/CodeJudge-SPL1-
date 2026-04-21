#include <stdio.h>

int main() {
    int a = 5, b = 10, c = 15;

    if (a > 0 && b > 0) {
        if (a < b || c > b) {
            for (int i = 0; i < a; i++) {
                while (b > 0) {
                    switch (i % 3) {
                        case 0: a += 1; break;
                        case 1: b -= 1; break;
                        default: c += 2; break;
                    }
                    b = (c > 20) ? b - 2 : b - 1;
                }
            }
        }
    }

    if (a > b) {
        c = (a > c) ? a : c;
    } else {
        c = (b > c) ? b : c;
    }
    return 0;
}
