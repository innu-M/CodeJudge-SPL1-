#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "metrics.h"

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define RESET "\033[0m"
#define BOLD "\033[1m"




//ED

int calculate_edit_distance(const char *str1, const char *str2) 
{
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    
    
    int **dp = (int **)malloc((len1 + 1) * sizeof(int *));
    for (int i = 0; i <= len1; i++) 
    {
        dp[i] = (int *)malloc((len2 + 1) * sizeof(int));
    }
    
   
    for (int i = 0; i <= len1; i++)
    
    dp[i][0] = i;
    for (int j = 0; j <= len2; j++) 
    dp[0][j] = j;
    
  
    for (int i = 1; i <= len1; i++) 
    {
        for (int j = 1; j <= len2; j++) 
        {
            if (str1[i-1] == str2[j-1]) 
            {
                dp[i][j] = dp[i-1][j-1];
            }
             else 
             {
                int insert = dp[i][j-1] + 1;
                int delete = dp[i-1][j] + 1;
                int replace = dp[i-1][j-1] + 1;
                
                dp[i][j] = (insert < delete) ? insert : delete;
                dp[i][j] = (dp[i][j] < replace) ? dp[i][j] : replace;
            }
        }
    }

     int result = dp[len1][len2];
    
     for (int i = 0; i <= len1; i++) {
        free(dp[i]);
    }
    free(dp);
    
    return result;
}




void find_lcs_and_highlight(const char *expected, const char *actual)
{
    int len1 = (int)strlen(expected);
    int len2 = (int)strlen(actual);

    // Build LCS DP table
    int **lcs = (int **)malloc((len1 + 1) * sizeof(int *));
    for (int i = 0; i <= len1; i++) {
        lcs[i] = (int *)calloc((size_t)len2 + 1, sizeof(int));
    }

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            if (expected[i - 1] == actual[j - 1]) {
                lcs[i][j] = lcs[i - 1][j - 1] + 1;
            } else {
                lcs[i][j] = (lcs[i - 1][j] > lcs[i][j - 1]) ? lcs[i - 1][j] : lcs[i][j - 1];
            }
        }
    }

    // Mark WHICH POSITIONS are in the LCS (this is the key fix)
    unsigned char *match_expected = (unsigned char *)calloc((size_t)len1, 1);
    unsigned char *match_actual   = (unsigned char *)calloc((size_t)len2, 1);

    int i = len1, j = len2;
    while (i > 0 && j > 0) {
        if (expected[i - 1] == actual[j - 1]) {
            match_expected[i - 1] = 1;
            match_actual[j - 1] = 1;
            i--; j--;
        } else if (lcs[i - 1][j] >= lcs[i][j - 1]) {
            i--;
        } else {
            j--;
        }
    }

    // Reconstruct LCS string (optional, just for display)
    int lcs_len = lcs[len1][len2];
    char *lcs_str = (char *)malloc((size_t)lcs_len + 1);
    lcs_str[lcs_len] = '\0';

    i = len1; j = len2;
    int idx = lcs_len;
    while (i > 0 && j > 0) {
        if (expected[i - 1] == actual[j - 1]) {
            lcs_str[--idx] = expected[i - 1];
            i--; j--;
        } else if (lcs[i - 1][j] >= lcs[i][j - 1]) {
            i--;
        } else {
            j--;
        }
    }

    printf(BOLD "\n========== OUTPUT DIFFERENCE VISUALIZATION ==========\n" RESET);
    printf(BLUE "LCS Length: %d\n" RESET, lcs_len);
    printf(BLUE "Common Subsequence: " RESET "%s\n\n", lcs_str);

    // Print Expected with RED blocks for mismatches
    printf(GREEN "Expected Output (differences in RED):\n" RESET);
    int in_red = 0;
    for (int k = 0; k < len1; k++) {
        char c = expected[k];

        if (c == '\n' || c == '\r') {
            if (in_red) { printf(RESET); in_red = 0; }
            putchar(c);
            continue;
        }

        if (!match_expected[k]) {
            if (!in_red) { printf(RED); in_red = 1; }
            putchar(c);
        } else {
            if (in_red) { printf(RESET); in_red = 0; }
            putchar(c);
        }
    }
    if (in_red) printf(RESET);
    printf("\n\n");

    // Print Actual with RED blocks for mismatches
    printf(YELLOW "Actual Output (differences in RED):\n" RESET);
    in_red = 0;
    for (int k = 0; k < len2; k++) {
        char c = actual[k];

        if (c == '\n' || c == '\r') {
            if (in_red) { printf(RESET); in_red = 0; }
            putchar(c);
            continue;
        }

        if (!match_actual[k]) {
            if (!in_red) { printf(RED); in_red = 1; }
            putchar(c);
        } else {
            if (in_red) { printf(RESET); in_red = 0; }
            putchar(c);
        }
    }
    if (in_red) printf(RESET);
    printf("\n");

    for (int k = 0; k <= len1; k++) free(lcs[k]);
    free(lcs);
    free(lcs_str);
    free(match_expected);
    free(match_actual);
}

void visualize_output_difference(const char *expected, const char *actual) {
    printf(BOLD "\n============ DETAILED OUTPUT ANALYSIS ============\n" RESET);
    

    int edit_dist = calculate_edit_distance(expected, actual);
    int max_len = (strlen(expected) > strlen(actual)) ? strlen(expected) : strlen(actual);
    double similarity = (max_len > 0) ? (1.0 - ((double)edit_dist / max_len)) * 100.0 : 100.0;
    
    printf("Expected Length: %lu characters\n", strlen(expected));
    printf("Actual Length: %lu characters\n", strlen(actual));
    printf(YELLOW "Edit Distance: %d\n" RESET, edit_dist);
    printf(BLUE "Similarity: %.2f%%\n" RESET, similarity);
    
    
    find_lcs_and_highlight(expected, actual);
    
    printf(BOLD "====================================================\n\n" RESET);
}