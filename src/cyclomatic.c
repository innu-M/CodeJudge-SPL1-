#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "metrics.h"

int calculate_cyclomatic_complexity(const char *filename, CodeMetrics *metrics) {
    FILE *file = fopen(filename, "r");
    if (!file)
     {
        printf("Error: Cannot open file for cyclomatic analysis\n");
        return 0;
    }
    
    char line[1024];
    int decision_points = 0;
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
    
        char *ptr = line;
       
    }
    
    fclose(file);
    
    
    metrics->decision_points = decision_points;
    metrics->cyclomatic_complexity = decision_points + 1;
    
    return 1;
}