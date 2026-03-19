#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "metrics.h"

int calculate_cyclomatic_complexity(const char *filename, CodeMetrics *metrics) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file for cyclomatic analysis\n");
        return 0;
    }
    
    char line[1024];
    int decision_points = 0;
    int in_comment = 0;
    
    while (fgets(line, sizeof(line), file)) {
        
        char *comment_start = strstr(line, "/*");
        char *comment_end = strstr(line, "*/");
        
        if (comment_start) {
            in_comment = 1;
            if (comment_end && comment_end > comment_start) {
                in_comment = 0;
            }
        } else if (comment_end) {
            in_comment = 0;
        }
        
        if (in_comment) continue;
        
       
        char *single_comment = strstr(line, "//");
        if (single_comment) {
            *single_comment = '\0';  
        }
        
        
        char *ptr = line;
        while (*ptr != '\0') {
            
            if (strncmp(ptr, "if", 2) == 0 && 
                (ptr == line || !isalnum(*(ptr-1))) && 
                !isalnum(ptr[2])) {
                decision_points++;
                ptr += 2;
                continue;
            }
            
           
            if (strncmp(ptr, "while", 5) == 0 && 
                (ptr == line || !isalnum(*(ptr-1))) && 
                !isalnum(ptr[5])) {
                decision_points++;
                ptr += 5;
                continue;
            }
            
            
            if (strncmp(ptr, "for", 3) == 0 && 
                (ptr == line || !isalnum(*(ptr-1))) && 
                !isalnum(ptr[3])) {
                decision_points++;
                ptr += 3;
                continue;
            }
            
            
            if (strncmp(ptr, "case", 4) == 0 && 
                (ptr == line || !isalnum(*(ptr-1))) && 
                !isalnum(ptr[4])) {
                decision_points++;
                ptr += 4;
                continue;
            }
            
            
            if (strncmp(ptr, "&&", 2) == 0) {
                decision_points++;
                ptr += 2;
                continue;
            }
            
            
            if (strncmp(ptr, "||", 2) == 0) {
                decision_points++;
                ptr += 2;
                continue;
            }
            
            
            if (*ptr == '?') {
                decision_points++;
            }
            
            ptr++;
        }
    }
    
    fclose(file);
    
    metrics->decision_points = decision_points;
    metrics->cyclomatic_complexity = decision_points + 1;
    
    return 1;
}