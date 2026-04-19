#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "metrics.h"

int calculate_cognitive_complexity(const char *filename, CodeMetrics *metrics) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file\n");
        return 0;
    }
    
    char line[1024];
    int cognitive_score = 0;
    int current_nesting = 0;
    int max_nesting = 0;
    int in_comment = 0;
    
    while (fgets(line, sizeof(line), file)) {
       char *comment_start = strstr(line, "/*");
        char *comment_end = strstr(line, "*/");
        
        if (comment_start)
         {
            in_comment = 1;
            if (comment_end && comment_end > comment_start) {
                in_comment = 0;
            }
        } 
        else if (comment_end) 
        {
            in_comment = 0;
        }
        
        if (in_comment) continue;
        
        char *single_comment = strstr(line, "//");
        if (single_comment) 
        {
            *single_comment = '\0';
        }
        
        for (int i = 0; line[i] != '\0'; i++) 
        {
            if (line[i] == '{') {
                current_nesting++;
                if (current_nesting > max_nesting) {
                    max_nesting = current_nesting;
                }
            }
            if (line[i] == '}') {
                current_nesting--;
                if (current_nesting < 0) current_nesting = 0;  
            }
        }
        
        
        char *ptr = line;
        

        while ((ptr = strstr(ptr, "if")) != NULL) 
        {
           if ((ptr == line || !isalnum(*(ptr-1))) && 
                !isalnum(ptr[2]) && 
                strchr(ptr, '(') != NULL) {
                if (current_nesting > 0) {
                    cognitive_score += (1 + (current_nesting - 1));
                } else {
                    cognitive_score += (1 + 0);
                }
            }
            ptr += 2;
        }
        
       
        ptr = line;
        while ((ptr = strstr(ptr, "else")) != NULL) 
        {
            if ((ptr == line || !isalnum(*(ptr-1))) && !isalnum(ptr[4])) {
                cognitive_score += 1;
            }
            ptr += 4;
        }
        
        
        ptr = line;
        while ((ptr = strstr(ptr, "while")) != NULL) 
        {
            if ((ptr == line || !isalnum(*(ptr-1))) && 
                !isalnum(ptr[5]) && 
                strchr(ptr, '(') != NULL) {
                if (current_nesting > 0) {
                    cognitive_score += 1 + (current_nesting - 1);
                } 
                else {
                    cognitive_score += 1 + 0;
                }
            }
            ptr += 5;
        }
        
       
        ptr = line;
        while ((ptr = strstr(ptr, "for")) != NULL) 
        {
            if ((ptr == line || !isalnum(*(ptr-1))) && 
                !isalnum(ptr[3]) && 
                strchr(ptr, '(') != NULL) {
                if (current_nesting > 0) {
                    cognitive_score += 1 + (current_nesting - 1);
                } 
                else {
                    cognitive_score += 1 + 0;
                }

            }
            ptr += 3;
        }
        
        
        ptr = line;
        while ((ptr = strstr(ptr, "switch")) != NULL) 
        {
            if ((ptr == line || !isalnum(*(ptr-1))) && !isalnum(ptr[6])) {
                if (current_nesting > 0) {
                    cognitive_score += (1 + (current_nesting - 1));
                } else {
                    cognitive_score += (1 + 0);
                }

            }
            ptr += 6;
        }
        
       
        ptr = line;
        while ((ptr = strstr(ptr, "case")) != NULL)
         {
            if ((ptr == line || !isalnum(*(ptr-1))) && !isalnum(ptr[4])) {
                cognitive_score += 1;
            }
            ptr += 4;
        }
        
        
        if (strstr(line, "break") || strstr(line, "continue")) 
        {
            cognitive_score += 1;
        }
        
       
        ptr = line;
        while ((ptr = strstr(ptr, "goto")) != NULL) 
        {
            if ((ptr == line || !isalnum(*(ptr-1))) && !isalnum(ptr[4])) {
                cognitive_score += 2;
            }
            ptr += 4;
        }
        
        
        ptr = line;
        while (*ptr != '\0') 
        {
            if (strncmp(ptr, "&&", 2) == 0 || strncmp(ptr, "||", 2) == 0) {
                cognitive_score += 1;
                ptr += 2;
            } else {
                ptr++;
            }
        }
        
        
        if (strchr(line, '?') != NULL && strchr(line, ':') != NULL) 
        {
            if (current_nesting > 0) {
                cognitive_score += (1 + (current_nesting - 1));
            } 
            else 
             {
            cognitive_score += (1 + 0);
            }

        }
    }
    
    fclose(file);
    
    metrics->cognitive_complexity = cognitive_score;
    metrics->max_nesting = max_nesting;
    metrics->nesting_level = current_nesting;  
    
    return 1;
}