#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "metrics.h"

// Unsafe functions that can cause buffer overflow
const char *unsafe_funcs[] = {
    "gets", "strcpy", "strcat", "sprintf", 
    "scanf", "fscanf", "sscanf",
    NULL
};

int detect_buffer_overflow_risk(const char *filename, CodeMetrics *metrics) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file for buffer overflow analysis\n");
        return 0;
    }
    
    char line[1024];
    int risk_score = 0;
    int unsafe_count = 0;
    int fixed_buffer_count = 0;
    int in_comment = 0;
    
    while (fgets(line, sizeof(line), file)) {
        // Skip comments
        if (strstr(line, "/*")) in_comment = 1;
        if (strstr(line, "*/")) {
            in_comment = 0;
            continue;
        }
        if (in_comment || strstr(line, "//")) continue;
        
        // Check for unsafe functions
        for (int i = 0; unsafe_funcs[i] != NULL; i++) {
            char pattern[50];
            snprintf(pattern, sizeof(pattern), "%s(", unsafe_funcs[i]);
            
            if (strstr(line, pattern)) {
                risk_score += 10;
                
                // Store function name if not already stored
                int already_stored = 0;
                for (int j = 0; j < unsafe_count; j++) {
                    if (strcmp(metrics->unsafe_functions[j], unsafe_funcs[i]) == 0) {
                        already_stored = 1;
                        break;
                    }
                }
                if (!already_stored && unsafe_count < 500) {
                    strcpy(metrics->unsafe_functions[unsafe_count], unsafe_funcs[i]);
                    unsafe_count++;
                }
            }
        }
        
        // Check for fixed-size buffers (char array[SIZE])
        if (strstr(line, "char") && strchr(line, '[') && strchr(line, ']')) {
            char *bracket_start = strchr(line, '[');
            char *bracket_end = strchr(line, ']');
            
            if (bracket_start && bracket_end && bracket_end > bracket_start) {
                // Check if it's a numeric size (fixed buffer)
                char size_str[20] = {0};
                int len = bracket_end - bracket_start - 1;
                if (len > 0 && len < 20) {
                    strncpy(size_str, bracket_start + 1, len);
                    if (isdigit(size_str[0])) {
                        fixed_buffer_count++;
                        risk_score += 5;
                    }
                }
            }
        }
        
        // Check for lack of bounds checking
        if (strstr(line, "strcpy") || strstr(line, "strcat")) {
            // Check if safer alternatives are NOT used
            if (!strstr(line, "strncpy") && !strstr(line, "strncat")) {
                risk_score += 5;
            }
        }
        
        // Check for scanf without width specifier
        if (strstr(line, "scanf") && strstr(line, "%s")) {
            // Check if there's no width limit like %20s
            char *format = strstr(line, "%s");
            if (format && !isdigit(*(format - 1))) {
                risk_score += 8;
            }
        }
    }
    
    fclose(file);
    
    metrics->buffer_overflow_risk = risk_score;
    metrics->unsafe_functions_count = unsafe_count;
    metrics->fixed_buffer_count = fixed_buffer_count;
    
    return 1;
}