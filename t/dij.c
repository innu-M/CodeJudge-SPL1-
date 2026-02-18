#include <stdio.h>
#include <stdlib.h>

typedef long long ll;

typedef struct { int to; int w; int next; } Edge;
typedef struct { ll d; int v; } HeapItem;

typedef struct {
    HeapItem *a;
    int sz;
} MinHeap;

static inline int read_int(int *out) {
    int c = getchar_unlocked();
    while (c != EOF && (c==' '||c=='\n'||c=='\r'||c=='\t')) c = getchar_unlocked();
    if (c == EOF) return 0;
    int sign = 1;
    if (c=='-') { sign=-1; c=getchar_unlocked(); }
    long long x=0;
    while (c>='0'&&c<='9') { x = x*10 + (c-'0'); c=getchar_unlocked(); }
    *out = (int)(x*sign);
    return 1;
}

static inline void swap_heap(HeapItem *x, HeapItem *y){ HeapItem t=*x; *x=*y; *y=t; }

static void heap_init(MinHeap *h, int cap){
    h->a = (HeapItem*)malloc((size_t)cap * sizeof(HeapItem));
    h->sz = 0;
}

static void heap_push(MinHeap *h, HeapItem it){
    int i = h->sz++;
    h->a[i] = it;
    while (i > 0) {
        int p = (i - 1) >> 1;
        if (h->a[p].d <= h->a[i].d) break;
        swap_heap(&h->a[p], &h->a[i]);
        i = p;
    }
}

static HeapItem heap_pop(MinHeap *h){
    HeapItem ret = h->a[0];
    h->a[0] = h->a[--h->sz];
    int i = 0;
    while (1) {
        int l = i*2+1, r = l+1, m = i;
        if (l < h->sz && h->a[l].d < h->a[m].d) m = l;
        if (r < h->sz && h->a[r].d < h->a[m].d) m = r;
        if (m == i) break;
        swap_heap(&h->a[m], &h->a[i]);
        i = m;
    }
    return ret;
}

int main(void){
    int n, m, s, q;
    if (!read_int(&n)) return 0;
    read_int(&m); read_int(&s); read_int(&q);

    int *head = (int*)malloc((size_t)(n+1) * sizeof(int));
    Edge *edges = (Edge*)malloc((size_t)(2LL*m) * sizeof(Edge));
    if (!head || !edges) return 0;
    for (int i=1;i<=n;i++) head[i] = -1;

    int ec = 0;
    for (int i=0;i<m;i++){
        int u,v,w;
        read_int(&u); read_int(&v); read_int(&w);
        edges[ec] = (Edge){v,w,head[u]}; head[u] = ec++;
        edges[ec] = (Edge){u,w,head[v]}; head[v] = ec++;
    }

    const ll INF = (ll)4e18;
    ll *dist = (ll*)malloc((size_t)(n+1) * sizeof(ll));
    unsigned char *vis = (unsigned char*)calloc((size_t)(n+1), 1);
    if (!dist || !vis) return 0;
    for (int i=1;i<=n;i++) dist[i] = INF;
    dist[s] = 0;

    MinHeap h;
    heap_init(&h, 2*m + 5);
    heap_push(&h, (HeapItem){0, s});

    while (h.sz) {
        HeapItem cur = heap_pop(&h);
        int u = cur.v;
        if (cur.d != dist[u]) continue;
        if (vis[u]) continue;
        vis[u] = 1;

        for (int e=head[u]; e!=-1; e=edges[e].next) {
            int v = edges[e].to;
            ll nd = dist[u] + (ll)edges[e].w;
            if (nd < dist[v]) {
                dist[v] = nd;
                heap_push(&h, (HeapItem){nd, v});
            }
        }
    }

    for (int i=0;i<q;i++){
        int t; read_int(&t);
        if (dist[t] >= INF/2) puts("-1");
        else printf("%lld\n", dist[t]);
    }

    free(head); free(edges); free(dist); free(vis); free(h.a);
    return 0;
}
