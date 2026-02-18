#include <stdio.h>
#include <stdlib.h>

typedef long long ll;

static inline int skip_ws(void) {
    int c = getchar_unlocked();
    while (c != EOF && (c==' '||c=='\n'||c=='\r'||c=='\t')) c = getchar_unlocked();
    return c;
}
static inline int read_int(int *out) {
    int c = skip_ws();
    if (c == EOF) return 0;
    int sign = 1;
    if (c=='-') { sign=-1; c=getchar_unlocked(); }
    long long x=0;
    while (c>='0'&&c<='9') { x = x*10 + (c-'0'); c=getchar_unlocked(); }
    *out = (int)(x*sign);
    return 1;
}
static inline void bit_add(ll *bit, int n, int i, ll delta) {
    while (i <= n) { bit[i] += delta; i += i & -i; }
}
static inline ll bit_sum(ll *bit, int i) {
    ll s=0;
    while (i > 0) { s += bit[i]; i -= i & -i; }
    return s;
}

int main(void) {
    int N, Q;
    if (!read_int(&N)) return 0;
    read_int(&Q);

    ll *bit = (ll*)calloc((size_t)N + 1, sizeof(ll));
    if (!bit) return 0;

    for (int i=1;i<=N;i++) {
        int x; read_int(&x);
        bit_add(bit, N, i, (ll)x);
    }

    for (int k=0;k<Q;k++) {
        int c = skip_ws();
        if (c == EOF) break;
        if (c == 'Q') {
            int l,r; read_int(&l); read_int(&r);
            ll ans = bit_sum(bit, r) - bit_sum(bit, l-1);
            printf("%lld\n", ans);
        } else if (c == 'U') {
            int idx, delta; read_int(&idx); read_int(&delta);
            bit_add(bit, N, idx, (ll)delta);
        } else {
            // skip bad line
            int ch;
            while ((ch=getchar_unlocked())!=EOF && ch!='\n');
            k--;
        }
    }

    free(bit);
    return 0;
}
