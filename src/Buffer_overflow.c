#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "metrics.h"

#define MAX_BUFFERS 100

typedef struct {
    char name[50];
    int declared_size;
    int is_tainted;
    int line_declared;
    char risky_ops[500];
} BufferTracker;

typedef struct {
    BufferTracker buffers[MAX_BUFFERS];
    int count;
    int overflow_count;
    int taint_violations;
} BufferAnalysis;

void extract_var_name(const char *line, char *var_name) {
    const char *ptr = strstr(line, "char");
    if (!ptr) return;
    
    ptr += 4;
    while (*ptr == ' ' || *ptr == '\t') ptr++;
    
    int i = 0;
    while (*ptr && (isalnum(*ptr) || *ptr == '_')) {
        var_name[i++] = *ptr++;
        if (i >= 49) break;
    }
    var_name[i] = '\0';
}

int extract_buffer_size(const char *line) {
    const char *start = strchr(line, '[');
    const char *end = strchr(line, ']');
    
    if (!start || !end || end <= start) 
    return -1;
    
    char size_str[20] = {0};
    int len = end - start - 1;
    
    if (len > 0 && len < 20) {
        strncpy(size_str, start + 1, len);
        if (isdigit(size_str[0])) {
            return atoi(size_str);
        }
    }
    return -1;
}

int is_taint_source(const char *line) {
    const char *sources[] = {"scanf", "gets", "fgets", "getchar", "fscanf", "sscanf", "read", "fread", NULL};
    
    for (int i = 0; sources[i] != NULL; i++) {
        if (strstr(line, sources[i])) return 1;
    }
    return 0;
}

int detect_buffer_overflow_risk(const char *filename, CodeMetrics *metrics) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file(buffer_overflow)\n");
        return 0;
    }
    
    BufferAnalysis analysis = {0};
    char line[1024];
    int line_num = 0;
    int in_comment = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        
        if (strstr(line, "/*")) in_comment = 1;
        if (strstr(line, "*/")) {
            in_comment = 0;
            continue;
        }
        if (in_comment || strstr(line, "//")) continue;
        
        if (strstr(line, "char") && strchr(line, '[') && strchr(line, ']')) {
            if (analysis.count < MAX_BUFFERS) {
                BufferTracker *buf = &analysis.buffers[analysis.count];
                
                extract_var_name(line, buf->name);
                buf->declared_size = extract_buffer_size(line);
                buf->line_declared = line_num;
                buf->is_tainted = 0;
                buf->risky_ops[0] = '\0';
                
                if (strlen(buf->name) > 0 && buf->declared_size > 0) {
                    analysis.count++;
                }
            }
        }
        
        if (is_taint_source(line)) {
            for (int i = 0; i < analysis.count; i++) {
                if (strstr(line, analysis.buffers[i].name)) {
                    analysis.buffers[i].is_tainted = 1;
                }
            }
        }
        
        for (int i = 0; i < analysis.count; i++) {
            BufferTracker *buf = &analysis.buffers[i];
            
            if (!strstr(line, buf->name)) continue;
            
            if (strstr(line, "strcpy")) {
                char *strcpy_pos = strstr(line, "strcpy");
                if (strstr(strcpy_pos, buf->name)) {
                    int copying_from_tainted = 0;
                    for (int j = 0; j < analysis.count; j++) {
                        if (i != j && analysis.buffers[j].is_tainted && strstr(line, analysis.buffers[j].name)) {
                            copying_from_tainted = 1;
                            break;
                        }
                    }
                    
                    if (copying_from_tainted || buf->is_tainted) {
                        analysis.overflow_count++;
                        analysis.taint_violations++;
                        char msg[100];
                        snprintf(msg, sizeof(msg), "Line %d: strcpy with tainted data; ", line_num);
                        strncat(buf->risky_ops, msg, sizeof(buf->risky_ops) - strlen(buf->risky_ops) - 1);
                    }
                }
            }
            
            if (strstr(line, "strcat") && !strstr(line, "strncat")) {
                if (buf->is_tainted) {
                    analysis.overflow_count++;
                    char msg[100];
                    snprintf(msg, sizeof(msg), "Line %d: strcat on tainted buffer; ", line_num);
                    strncat(buf->risky_ops, msg, sizeof(buf->risky_ops) - strlen(buf->risky_ops) - 1);
                }
            }
            
            if (strstr(line, "sprintf") && !strstr(line, "snprintf")) {
                if (buf->is_tainted) {
                    analysis.overflow_count++;
                    char msg[100];
                    snprintf(msg, sizeof(msg), "Line %d: sprintf on tainted buffer; ", line_num);
                    strncat(buf->risky_ops, msg, sizeof(buf->risky_ops) - strlen(buf->risky_ops) - 1);
                }
            }
            
            if (strstr(line, "gets")) {
                analysis.overflow_count += 2;
                analysis.taint_violations++;
                char msg[100];
                snprintf(msg, sizeof(msg), "Line %d: gets() CRITICAL; ", line_num);
                strncat(buf->risky_ops, msg, sizeof(buf->risky_ops) - strlen(buf->risky_ops) - 1);
            }
            
            if (strstr(line, "scanf") && strstr(line, "%s")) {
                char *fmt = strstr(line, "%s");
                if (fmt && (fmt == line || !isdigit(*(fmt - 1)))) {
                    analysis.overflow_count++;
                    analysis.taint_violations++;
                    char msg[100];
                    snprintf(msg, sizeof(msg), "Line %d: scanf without width; ", line_num);
                    strncat(buf->risky_ops, msg, sizeof(buf->risky_ops) - strlen(buf->risky_ops) - 1);
                }
            }
        }
    }
    
    fclose(file);
    
    printf("\n========== BUFFER OVERFLOW ANALYSIS ==========\n");
    printf("Buffers tracked: %d\n", analysis.count);
    printf("Potential overflows: %d\n", analysis.overflow_count);
    printf("Taint violations: %d\n\n", analysis.taint_violations);
    
    if (analysis.count > 0) {
        printf("Buffer Details:\n");
        printf("%-15s %-8s %-8s %s\n", "Name", "Size", "Tainted", "Risky Operations");
       
        
        for (int i = 0; i < analysis.count; i++) {
            BufferTracker *buf = &analysis.buffers[i];
            printf("%-15s %-8d %-8s %s\n", 
                   buf->name, 
                   buf->declared_size,
                   buf->is_tainted ? "YES" : "NO",
                   strlen(buf->risky_ops) > 0 ? buf->risky_ops : "None");
        }
    }
    
    metrics->buffer_overflow_risk = analysis.overflow_count * 10 + analysis.taint_violations * 5;
    metrics->unsafe_functions_count = analysis.overflow_count;
    metrics->fixed_buffer_count = analysis.count;
    
    printf("\nRisk Level: ");
    if (metrics->buffer_overflow_risk < 10) {
        printf("\033[1;32mLOW\033[0m\n");
    }
     else if (metrics->buffer_overflow_risk < 30) 
     {
        printf("\033[1;33mMEDIUM\033[0m\n");
    } 
    else {
        printf("\033[1;31mHIGH\033[0m\n");
    }
    
    
    return 1;
}