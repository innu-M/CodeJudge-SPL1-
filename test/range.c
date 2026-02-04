#include <stdio.h>
#include <stdlib.h>

int main() {
    int N, Q;
    if (scanf("%d %d", &N, &Q) != 2) return 0;

    long long *pref = (long long*)malloc((N + 1) * sizeof(long long));
    if (!pref) return 0;
    pref[0] = 0;

    for (int i = 1; i <= N; i++) {
        long long x;
        scanf("%lld", &x);
        pref[i] = pref[i - 1] + x;
    }

    while (Q--) {
        int l, r;
        scanf("%d %d", &l, &r);
        printf("%lld\n", pref[r] - pref[l - 1]);
    }

    free(pref);
    return 0;
}
