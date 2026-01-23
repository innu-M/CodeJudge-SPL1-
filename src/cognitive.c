#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "metrics.h"

int calculate_cognitive_complexity(const char *filename, CodeMetrics *metrics) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file for cognitive analysis\n");
        return 0;
    }
    
    char line[1024];
    int cognitive_score = 0;
    int current_nesting = 0;
    int max_nesting = 0;
    int in_comment = 0;
    
    while (fgets(line, sizeof(line), file)) 
    {
        
        if (strstr(line, "/*")) in_comment = 1;
        if (strstr(line, "*/"))
         {
            in_comment = 0;
            continue;
        }
        if (in_comment || strstr(line, "//")) 
        continue;
        
        // Track nesting level
        for (int i = 0; line[i]; i++)
         {
            if (line[i] == '{') current_nesting++;
            if (line[i] == '}') current_nesting--;
        }
        
        if (current_nesting > max_nesting) 
        {
            max_nesting = current_nesting;
        }
        
        char *ptr = line;
        
        if (strstr(ptr, "if") && strchr(ptr, '(')) 
        {
            cognitive_score += (1 + current_nesting);
        }
        else if (strstr(ptr, "else")) 
        {
            cognitive_score += 1;
        }
        else if (strstr(ptr, "while") || strstr(ptr, "for"))
         {
            cognitive_score += (1 + current_nesting);
        }
        else if (strstr(ptr, "switch")) 
        {
            cognitive_score += (1 + current_nesting);
        }
        else if (strstr(ptr, "case")) 
        {
            cognitive_score += 1;
        }
        else if (strstr(ptr, "break") || strstr(ptr, "continue"))
         {

            cognitive_score += 1;
        }
        else if (strstr(ptr, "goto")) 
        {
            cognitive_score += 2;     }
        
        // Logical operators
        char *logical = ptr;
        while ((logical = strstr(logical, "&&")) != NULL || 
               (logical = strstr(logical, "||")) != NULL) {
            cognitive_score += 1;
            logical += 2;
        }
        
        // Nested 
        if (strchr(ptr, '?')) {
            cognitive_score += (1 + current_nesting);
        }
        
       }
    
    fclose(file);
    
    metrics->cognitive_complexity = cognitive_score;
    metrics->max_nesting = max_nesting;
    
    return 1;
}