#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "metrics.h"

#define RED    "\033[1;31m"
#define GREEN  "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE   "\033[1;34m"
#define RESET  "\033[0m"
#define BOLD   "\033[1m"



static size_t count_lines(const char *s) {
    if (!s || !*s) return 0;
    size_t lines = 1;
    for (const char *p = s; *p; p++) if (*p == '\n') lines++;
    return lines;
}

static size_t first_mismatch_index(const char *a, const char *b) {
    size_t i = 0;
    while (a[i] && b[i] && a[i] == b[i]) i++;
    if (!a[i] && !b[i]) return (size_t)-1; // identical
    return i;
}

static void line_col_at(const char *s, size_t idx, size_t *line, size_t *col) {
    size_t l = 1, c = 1;
    for (size_t i = 0; i < idx && s[i]; i++) {
        if (s[i] == '\n') { l++; c = 1; }
        else c++;
    }
    *line = l; *col = c;
}

static const char* line_start_at(const char *s, size_t idx) {
    const char *p = s + idx;
    while (p > s && p[-1] != '\n') p--;
    return p;
}

static const char* line_end_at(const char *s, size_t idx) {
    const char *p = s + idx;
    while (*p && *p != '\n') p++;
    return p;
}

static void print_line_with_caret(const char *prefix, const char *line_start, const char *line_end, size_t caret_col_1based) {
    
    const size_t MAX_SHOW = 160;

    size_t line_len = (size_t)(line_end - line_start);
    size_t caret0 = (caret_col_1based > 0 ? caret_col_1based - 1 : 0);

    size_t show_start = 0;
    size_t show_len = line_len;

    if (line_len > MAX_SHOW) {
        
        size_t half = MAX_SHOW / 2;
        if (caret0 > half) show_start = caret0 - half;
        if (show_start + MAX_SHOW > line_len) show_start = line_len - MAX_SHOW;
        show_len = MAX_SHOW;
    }

    
    printf("%s", prefix);
    fwrite(line_start + show_start, 1, show_len, stdout);
    putchar('\n');

   
    if (caret0 >= show_start && caret0 < show_start + show_len) {
        size_t caret_in_slice = caret0 - show_start;
        
        size_t prefix_len = strlen(prefix);
        for (size_t i = 0; i < prefix_len + caret_in_slice; i++) putchar(' ');
        printf(RED "^\n" RESET);
    }
    if (line_len > MAX_SHOW) {
        printf(BLUE "  [line truncated]\n" RESET);
    }
}

static void print_context_window(const char *label, const char *s, size_t mismatch_line, size_t mismatch_col,
                                 size_t ctx_before, size_t ctx_after, int caret_here) {
    printf(BOLD "%s\n" RESET, label);

    size_t cur_line = 1;
    const char *p = s;

    while (*p) {
        const char *ls = p;
        const char *le = p;
        while (*le && *le != '\n') le++;

        if (cur_line + ctx_after < mismatch_line) {
            
            p = (*le == '\n') ? le + 1 : le;
            cur_line++;
            continue;
        }
        if (cur_line + ctx_before > mismatch_line + ctx_after) break;

      
        char prefix[64];
        snprintf(prefix, sizeof(prefix), "  %5zu | ", cur_line);

        if (caret_here && cur_line == mismatch_line) {
            
            print_line_with_caret(prefix, ls, le, mismatch_col);
        } else {
            
            size_t len = (size_t)(le - ls);
            const size_t MAX_SHOW = 160;
            printf("%s", prefix);
            if (len <= MAX_SHOW) fwrite(ls, 1, len, stdout);
            else { fwrite(ls, 1, MAX_SHOW, stdout); printf(BLUE " ...[truncated]" RESET); }
            putchar('\n');
        }

      
        p = (*le == '\n') ? le + 1 : le;
        if (*le == '\0') break;
        cur_line++;
    }
}

void visualize_output_difference(const char *expected, const char *actual) {
    size_t len_e = strlen(expected);
    size_t len_a = strlen(actual);
    size_t lines_e = count_lines(expected);
    size_t lines_a = count_lines(actual);

    printf(BOLD "\n============ OUTPUT DIFF (CLEAR VIEW) ============\n" RESET);
    printf("Expected: %zu chars, %zu lines\n", len_e, lines_e);
    printf("Actual:   %zu chars, %zu lines\n", len_a, lines_a);

    size_t mm = first_mismatch_index(expected, actual);
    if (mm == (size_t)-1) {
        printf(GREEN "\nOutputs match exactly.\n" RESET);
        printf(BOLD "==================================================\n\n" RESET);
        return;
    }


    size_t le, ce, la, ca;
    line_col_at(expected, mm, &le, &ce);
    line_col_at(actual,   mm, &la, &ca);

    printf(RED "\nFirst mismatch at index %zu\n" RESET, mm);

    
    if (expected[mm] == '\0') printf(YELLOW "Expected ended early (EOF), but actual continues.\n" RESET);
    else if (actual[mm] == '\0') printf(YELLOW "Actual ended early (EOF), but expected continues.\n" RESET);
    else {
        unsigned char e = (unsigned char)expected[mm];
        unsigned char a = (unsigned char)actual[mm];
        printf("Expected char: '%c' (0x%02X)\n", (e>=32 && e<127)?e:'.', e);
        printf("Actual char:   '%c' (0x%02X)\n", (a>=32 && a<127)?a:'.', a);
    }

    printf("\nExpected location: line %zu, col %zu\n", le, ce);
    printf("Actual location:   line %zu, col %zu\n\n", la, ca);


    const size_t CTX_BEFORE = 2, CTX_AFTER = 2;
    print_context_window("Expected (context):", expected, le, ce, CTX_BEFORE, CTX_AFTER, 1);
    printf("\n");
    print_context_window("Actual (context):", actual, la, ca, CTX_BEFORE, CTX_AFTER, 1);

    printf(BOLD "\nTip:" RESET " If this is a big output, fix the first mismatch first—later mismatches often disappear.\n");
    
}
