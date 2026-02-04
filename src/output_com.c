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




void find_lcs_and_highlight(const char *expected, const char *actual) {
    int len1 = strlen(expected);
    int len2 = strlen(actual);
    
    
    int **lcs = (int **)malloc((len1 + 1) * sizeof(int *));
    for (int i = 0; i <= len1; i++) {
        lcs[i] = (int *)calloc(len2 + 1, sizeof(int));
    }
    
    
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            if (expected[i-1] == actual[j-1]) {
                lcs[i][j] = lcs[i-1][j-1] + 1;
            } else {
                lcs[i][j] = (lcs[i-1][j] > lcs[i][j-1]) ? lcs[i-1][j] : lcs[i][j-1];
            }
        }
    }
    
   
    char *lcs_str = (char *)malloc((len1 + len2 + 1) * sizeof(char));
    int index = lcs[len1][len2];
    lcs_str[index] = '\0';
    
    int i = len1, j = len2;
    while (i > 0 && j > 0) {
        if (expected[i-1] == actual[j-1]) {
            lcs_str[--index] = expected[i-1];
            i--;
            j--;
        } else if (lcs[i-1][j] > lcs[i][j-1]) {
            i--;
        } else {
            j--;
        }
    }
    
    printf(BOLD "\n========== OUTPUT DIFFERENCE VISUALIZATION ==========\n" RESET);
    printf(BLUE "LCS Length: %d\n" RESET, lcs[len1][len2]);
    printf(BLUE "Common Substring: " RESET "%s\n\n", lcs_str);
    
   printf(GREEN "Expected Output (differences in RED):\n" RESET);
    for (int k = 0; k < len1; k++) {
        int found_in_lcs = 0;
        for (int m = 0; lcs_str[m] != '\0'; m++) {
            if (expected[k] == lcs_str[m]) {
                found_in_lcs = 1;
                break;
            }
        }
        
        if (found_in_lcs) {
            printf("%c", expected[k]);
        } else {
            printf(RED "%c" RESET, expected[k]);
        }
    }
    printf("\n\n");
    

    printf(YELLOW "Actual Output (differences in RED):\n" RESET);
    for (int k = 0; k < len2; k++) {
        int found_in_lcs = 0;
        for (int m = 0; lcs_str[m] != '\0'; m++) {
            if (actual[k] == lcs_str[m]) {
                found_in_lcs = 1;
                break;
            }
        }
        
        if (found_in_lcs) {
            printf("%c", actual[k]);
        } else {
            printf(RED "%c" RESET, actual[k]);
        }
    }
    printf("\n");
    
    for (int k = 0; k <= len1; k++) {
        free(lcs[k]);
    }
    free(lcs);
    free(lcs_str);
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