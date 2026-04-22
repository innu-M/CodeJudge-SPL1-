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
#define MAX_SHOW 160



int count_lines(const char *s)
{
    if (!s) 
    return 0;
    int lines = 1;
    for (const char *p = s; *p; p++)
        if (*p == '\n')
            lines++;
    return lines;
}

int first_mismatch_index(const char *a, const char *b) 
{
    int i = 0;
    while (a[i] && b[i] && a[i] == b[i])
        i++;
    if (!a[i] && !b[i])
        return -1; // identical
    return i;
}

void line_col_at(const char *s, int idx, int *line, int *col) 
{
    int l = 1, c = 1;
    for (int i = 0; i < idx && s[i]; i++) {
        if (s[i] == '\n') 
        { l++; c = 1; }
         else
         c++;
    }
    *line = l; *col = c;
}

const char* line_start_at(const char *s, int idx) 
{
    const char *p = s + idx;
    while (p > s && p[-1] != '\n') 
    p--;
    return p;
}

const char* line_end_at(const char *s, int idx) 
{
    const char *p = s + idx;
    while (*p && *p != '\n') 
    p++;
    return p;
}

void print_line_with_caret(const char *prefix, const char *line_start, const char *line_end, int caret_col_1based) 
{
    int line_len = (int)(line_end - line_start);
    int caret0;

    if (caret_col_1based > 0)
        caret0 = caret_col_1based - 1;
    else
        caret0 = 0;

    int show_start = 0;
    int show_len = line_len;

    if (line_len > MAX_SHOW) {
        int half = MAX_SHOW / 2;
        if (caret0 > half)
            show_start = caret0 - half;
        if (show_start + MAX_SHOW > line_len)
            show_start = line_len - MAX_SHOW;
        show_len = MAX_SHOW;
    }

    printf("%s", prefix);
    fwrite(line_start + show_start, 1, show_len, stdout);
    putchar('\n');

    if (caret0 >= show_start && caret0 < show_start + show_len) {
        int caret_in_slice = caret0 - show_start;
        int prefix_len = (int)strlen(prefix);
        for (int i = 0; i < prefix_len + caret_in_slice; i++)
            putchar(' ');
        printf(RED "^\n" RESET);
    }
    if (line_len > MAX_SHOW) {
        printf(BLUE "  [line truncated]\n" RESET);
    }
}

void print_context_window(const char *label, const char *s, int mismatch_line, int mismatch_col,
                          int ctx_before, int ctx_after, int caret_here) 
                          {
    printf(BOLD "%s\n" RESET, label);

    int cur_line = 1;
    const char *p = s;

    while (*p) {
        const char *ls = p;
        const char *le = p;
        while (*le && *le != '\n') 
        le++;

        if (cur_line + ctx_after < mismatch_line) 
        {
            if (*le == '\n')
                p = le + 1;
            else
                p = le;
            cur_line++;
            continue;
        }

        if (cur_line + ctx_before > mismatch_line + ctx_after)
            break;

        char prefix[64];
        snprintf(prefix, sizeof(prefix), "  %5d | ", cur_line);

        if (caret_here && cur_line == mismatch_line) 
        {
            print_line_with_caret(prefix, ls, le, mismatch_col);
        }
         else 
         {
            int len = (int)(le - ls);
            printf("%s", prefix);
            if (len <= MAX_SHOW)
                fwrite(ls, 1, len, stdout);
            else {
                fwrite(ls, 1, MAX_SHOW, stdout);
                printf(BLUE " ...[truncated]" RESET);
            }
            putchar('\n');
        }

        if (*le == '\n')
            p = le + 1;
        else
            p = le;

        if (*le == '\0') break;
        cur_line++;
    }
}

void visualize_output_difference(const char *expected, const char *actual) {
    int len_e = (int)strlen(expected);
    int len_a = (int)strlen(actual);
    int lines_e = count_lines(expected);
    int lines_a = count_lines(actual);

    printf(BOLD "\n---------------------- OUTPUT DIFFERENCE----------------------------\n" RESET);
    printf("Expected: %d chars, %d lines\n", len_e, lines_e);
    printf("Actual:   %d chars, %d lines\n", len_a, lines_a);

    int mm = first_mismatch_index(expected, actual);
    if (mm == -1) {
        printf(GREEN "\nOutputs match exactly.\n" RESET);
        printf("\n\n");
        return;
    }

    int le, ce, la, ca;
    line_col_at(expected, mm, &le, &ce);
    line_col_at(actual,   mm, &la, &ca);

    printf(RED "\nFirst mismatch at index %d\n" RESET, mm);

    if (expected[mm] == '\0') {
        printf(YELLOW "Expected ended early (EOF), but actual continues.\n" RESET);
    }
     else if (actual[mm] == '\0') 
     {
        printf(YELLOW "Actual ended early (EOF), but expected continues.\n" RESET);
    }
     else 
     {
        printf("Expected char: '%c'\n", expected[mm]);
        printf("Actual char:   '%c'\n", actual[mm]);
    }

    printf("\nExpected location: line %d, col %d\n", le, ce);
    printf("Actual location:   line %d, col %d\n\n", la, ca);

    const int CTX_BEFORE = 2, CTX_AFTER = 2;
    
    print_context_window("Expected (context):", expected, le, ce, CTX_BEFORE, CTX_AFTER, 1);
    printf("\n");
    print_context_window("Actual (context):", actual, la, ca, CTX_BEFORE, CTX_AFTER, 1);

    printf(BOLD "\nTip:" RESET " If this is a big output, fix the first mismatch first—later mismatches often disappear.\n");
}