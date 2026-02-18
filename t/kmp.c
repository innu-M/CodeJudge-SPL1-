#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char *read_line_dynamic(void) {
    size_t cap = 1024, len = 0;
    char *buf = (char*)malloc(cap);
    if (!buf) return NULL;

    int c;
    while ((c = getchar_unlocked()) != EOF) {
        if (c == '\r') continue;
        if (c == '\n') break;
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = (char*)realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = (char)c;
    }
    if (c == EOF && len == 0) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    char *pat = read_line_dynamic();
    if (!pat) return 0;
    char *txt = read_line_dynamic();
    if (!txt) { free(pat); return 0; }

    int m = (int)strlen(pat);
    int n = (int)strlen(txt);

    int *pi = (int*)calloc((size_t)m, sizeof(int));
    if (!pi) { free(pat); free(txt); return 0; }

    // prefix function
    for (int i=1, j=0; i<m; i++) {
        while (j>0 && pat[i] != pat[j]) j = pi[j-1];
        if (pat[i] == pat[j]) j++;
        pi[i] = j;
    }

    long long count = 0;
    int first[20];
    int fsz = 0;

    for (int i=0, j=0; i<n; i++) {
        while (j>0 && txt[i] != pat[j]) j = pi[j-1];
        if (txt[i] == pat[j]) {
            j++;
            if (j == m) {
                int start = i - m + 1;
                count++;
                if (fsz < 20) first[fsz++] = start;
                j = pi[j-1];
            }
        }
    }

    printf("%lld\n", count);
    for (int i=0; i<fsz; i++) printf("%d\n", first[i]);

    free(pi);
    free(pat);
    free(txt);
    return 0;
}
