#include <stdio.h>

int main() {
    int T;
    if (scanf("%d", &T) != 1) return 0;

    while (T--) {
        int N;
        scanf("%d", &N);

        long long sum = 0;
        long long mn = 0, mx = 0;

        for (int i = 0; i < N; i++) {
            long long x;
            scanf("%lld", &x);
            if (i == 0) mn = mx = x;
            if (x < mn) mn = x;
            if (x > mx) mx = x;
            sum += x;
        }

        printf("%lld %lld %lld\n", sum, mn, mx);
    }
    return 0;
}
