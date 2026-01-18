#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "metrics.h"
#include "mathlib.h"

// Operator and keyword lists
const char *operators[] = {
    "+", "-", "*", "/", "%", "++", "--",
    "=", "+=", "-=", "*=", "/=", "%=",
    "==", "!=", "<", ">", "<=", ">=",
    "&&", "||", "!",
    "&", "|", "^", "~", "<<", ">>",
    "?", ":", ",", ".", "->",
    "sizeof", "return", "if", "else", "while", 
    "for", "do", "switch", "case", "break", 
    "continue", "goto", "default",
    NULL
};

const char *keywords[] = {
    "auto", "break", "case", "char", "const", "continue",
    "default", "do", "double", "else", "enum", "extern",
    "float", "for", "goto", "if", "int", "long",
    "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void",
    "volatile", "while",
    NULL
};

int is_operator(const char *token) 
{
    for (int i = 0; operators[i] != NULL; i++) 
    {
        if (strcmp(token, operators[i]) == 0) 
        {
            return 1;
        }
    }
    return 0;
}

int is_keyword(const char *token) 
{
    for (int i = 0; keywords[i] != NULL; i++) 
    {
        if (strcmp(token, keywords[i]) == 0) 
        {
            return 1;
        }
    }
    return 0;
}

int analyze_source_file(const char *filename, CodeMetrics *metrics) 
{
    FILE *file = fopen(filename, "r");

    if (!file) 
    {
        printf("Error: Cannot open file %s\n", filename);
        return 0;
    }


    memset(metrics, 0, sizeof(CodeMetrics));
    
    char line[1024];
    int in_comment_block = 0;
    int in_string = 0;
    int brace_count = 0;
    
   
    char *distinct_ops[100] = {0};
    char *distinct_operands[100] = {0};
    int op_count = 0;
    int operand_count = 0;
    
    while (fgets(line, sizeof(line), file)) 
    {
        metrics->loc++;
        
      
        line[strcspn(line, "\n")] = 0;
        
        
        char *trimmed = line;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
        
       
        if (in_comment_block) {
            metrics->comments++;
            char *end_comment = strstr(trimmed, "*/");

            if (end_comment) 
            {
                in_comment_block = 0;
             
                char *after_comment = end_comment + 2;
                while (*after_comment == ' ' || *after_comment == '\t') after_comment++;

                if (*after_comment != '\0') 
                {
                    metrics->lloc++;
                }
            }
            continue;
        }
        
        
        char *start_comment = strstr(trimmed, "/*");
        if (start_comment) 
        {
            metrics->comments++;
            in_comment_block = 1;
            char *end_comment = strstr(start_comment + 2, "*/");
            if (end_comment)
            {
                in_comment_block = 0;
            }
            
            if (start_comment > trimmed) {
                metrics->lloc++;
            }
            continue;
        }
        
       
        char *line_comment = strstr(trimmed, "//");
        if (line_comment) {
            metrics->comments++;
           
            if (line_comment > trimmed)
            {
                metrics->lloc++;
            }
            continue;
        }
        
       
        if (strlen(trimmed) == 0) 
        {
            continue;
        }
        
        metrics->lloc++;
        
       
        char *token = strtok(line, " \t\n\r{}();,=+-*/%&|!<>");
        while (token) 
        {
           
            if (isdigit(token[0])) 
            {
                token = strtok(NULL, " \t\n\r{}();,=+-*/%&|!<>");
                continue;
            }
            

            if (is_operator(token) || is_keyword(token)) 
            {
                metrics->halstead_N1++;
                
  
                int found = 0;
                for (int i = 0; i < op_count; i++) 
                {
                    if (strcmp(distinct_ops[i], token) == 0) 
                    {
                        found = 1;
                        break;
                    }
                }
                if (!found && op_count < 100) 
                {
                    distinct_ops[op_count] = strdup(token);
                    op_count++;
                    metrics->halstead_n1++;
                }
            }
             else 
            {
                
                metrics->halstead_N2++;
                
                
                int found = 0;
                for (int i = 0; i < operand_count; i++) 
                {
                    if (distinct_operands[i] && strcmp(distinct_operands[i], token) == 0) 
                    {
                        found = 1;
                        break;
                    }
                }
                if (!found && operand_count < 100) 
                {
                    distinct_operands[operand_count] = strdup(token);
                    operand_count++;
                    metrics->halstead_n2++;
                }
            }
            
            token = strtok(NULL, " \t\n\r{}();,=+-*/%&|!<>");
        }
        
  
        if (strstr(trimmed, "(") && strstr(trimmed, ")") && 
            !strstr(trimmed, ";") && strstr(trimmed, "{")) 
            {
            
            metrics->functions++;
        }
        
     
        for (int i = 0; trimmed[i]; i++) 
        {
            if (trimmed[i] == '{') brace_count++;
            else if (trimmed[i] == '}') brace_count--;
        }
    }
    
    fclose(file);

    metrics->halstead_length = metrics->halstead_N1 + metrics->halstead_N2;
    metrics->halstead_vocab = metrics->halstead_n1 + metrics->halstead_n2;
    
    if (metrics->halstead_vocab > 0) 
    {
        metrics->halstead_volume = metrics->halstead_length * 
                                  (log(metrics->halstead_vocab) / log(2));
    }
    
    if (metrics->halstead_n2 > 0)
     {
        metrics->halstead_difficulty = ((double)metrics->halstead_n1 / 2.0) * 
                                      ((double)metrics->halstead_N2 / metrics->halstead_n2);
    }
    
    metrics->halstead_effort = metrics->halstead_difficulty * metrics->halstead_volume;
    
    

    for (int i = 0; i < op_count; i++)
     free(distinct_ops[i]);

    for (int i = 0; i < operand_count; i++)
     free(distinct_operands[i]);
    
    return 1;
}

void display_metrics(const CodeMetrics *metrics) {
    printf("\n              ============= CODE METRICS ANALYSIS ============    \n");
    printf("                      Lines of Code (LOC): %d\n", metrics->loc);

    printf("                      Logical LOC: %d\n", metrics->lloc);
    printf("                      Comment lines: %d\n", metrics->comments);

    printf("                      Comment percentage: %.1f%%\n", 
           (metrics->comments * 100.0) / metrics->loc);
    printf("                      Number of functions: %d\n", metrics->functions);
  
  //halstead metrics
    printf("\n             ======= Halstead Metrics =========       \n");


    /* printf("Distinct operators (n1): %d\n", metrics->halstead_n1);
    printf("Distinct operands (n2): %d\n", metrics->halstead_n2);
    printf("Total operators (N1): %d\n", metrics->halstead_N1);
    printf("Total operands (N2): %d\n", metrics->halstead_N2);
    printf("Program length (N): %.0f\n", metrics->halstead_length);

        */
    printf("                      Vocabulary size (n): %.0f\n", metrics->halstead_vocab);
    printf("                      Volume (V): %.2f\n", metrics->halstead_volume);
    printf("                      Difficulty (D): %.2f\n", metrics->halstead_difficulty);
    printf("                      Effort (E): %.2f\n", metrics->halstead_effort);


     //comments           
     
}