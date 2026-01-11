#include<sys/wait.h>
#include<signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include "metrics.h"
//#include "detailed_com.h"

#define LIM 2000

void clear_buffer();
void read_file(char *filename, char *content);
void get_user_input(char *buffer);
int compile_program(char *source_file);
void get_input_data(char *input);
void get_expected_output(char *expected);
void save_input_to_file(char *input);
void run_program_and_capture_output(char *actual);
void display_comparison(char *input, char *expected, char *actual);
int execute_test_case(int test_number);
void cleanup_test_files();
void display_final_result(int passed, int total);


//metrics
void analyze_code_metrics(char *source_file) {
    printf("\n          ANALYZING CODE METRICS\n");
    
    CodeMetrics metrics;
    if (analyze_source_file(source_file, &metrics)) {
        display_metrics(&metrics);
    } else {
        printf("Failed to analyze code metrics.\n");
    }
}





int main() {
    printf("                            CODEJUDGE\n\n");
    
    char source_file[256];
    printf("Enter C file path: ");
    scanf("%s", source_file);
    clear_buffer();

    if (!compile_program(source_file)) 
    {
        return 1;
    }
    
    printf("\nHow many test cases? ");
    int test_count;
    scanf("%d", &test_count);
    clear_buffer();
    
    int passed = 0;
    
    for (int i = 1; i <= test_count; i++) 
    {
        passed += execute_test_case(i);
        cleanup_test_files();
    }
    
    display_final_result(passed, test_count);

    //analysis
    printf("\nPerform code metrics analysis? (1=Yes, 0=No): ");
    int analyze_metrics;
    scanf("%d", &analyze_metrics);
    clear_buffer();
    
    if (analyze_metrics) {
        analyze_code_metrics(source_file);
    }
    //will remove this part
    if (!compile_program(source_file)) {
        return 1;
    }
    remove("myprogram");
    
    return 0;
}


void clear_buffer() 
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void read_file(char *filename, char *content) 
{
    FILE *file = fopen(filename, "r");

    if (file == NULL) 
    {
        printf("File not found: %s\n", filename);
        strcpy(content, "");
        return;
    }
    
    content[0] = '\0';
    char line[200];
    int first = 1;
    
    while (fgets(line, sizeof(line), file) != NULL) 
    {
        if (!first) 
        {
            strcat(content, "\n");
        }
        line[strcspn(line, "\n")] = '\0';
        strcat(content, line);
        first = 0;
    }
    
    fclose(file);
}

void get_user_input(char *buffer) 
{
    buffer[0] = '\0';
    char line[200];
    
    printf("Enter number of lines: ");
    int lines;
    scanf("%d", &lines);
    clear_buffer();
    
    for (int i = 0; i < lines; i++) 
    {
        printf("Line %d: ", i + 1);
        fgets(line, sizeof(line), stdin);
        line[strcspn(line, "\n")] = '\0';
        
        if (i > 0) 
        {
            strcat(buffer, "\n");
        }
        strcat(buffer, line);
    }
}

int compile_program(char *source_file) 
{
    char command[512];
    printf("Compiling %s\n", source_file);
    sprintf(command, "gcc %s -o myprogram 2> error.txt", source_file);
    system(command);

    FILE *error_file = fopen("error.txt", "r");
    if (error_file != NULL) 
    {
        char ch;
        int has_error = 0;
        printf("Compilation output:\n");
        while ((ch = fgetc(error_file)) != EOF) 
        {
            putchar(ch);
            has_error = 1;
        }
        fclose(error_file);
        
        if (has_error) 
        {
            printf("Compilation FAILED!\n");
            remove("error.txt");
            return 0;
        }
    }
    remove("error.txt");
    printf("Compilation SUCCESS!\n");
    return 1;
}

void get_input_data(char *input) 
{
    printf("Input from:\n");
    printf(" 1.Keyboard\n");
    printf(" 2.File\n");
    int method;
    scanf("%d", &method);
    clear_buffer();
    
    if (method == 1) 
    {
        get_user_input(input);
    }
     else 
     {
        printf("Enter input file path\n ");
        char input_file[256];
        fgets(input_file, sizeof(input_file), stdin);
        input_file[strcspn(input_file, "\n")] = '\0';
        read_file(input_file, input);
    }
}



void get_expected_output(char *expected) 
{
    printf("\nExpected output from:\n");
    printf(" 1.Keyboard\n");
    printf(" 2.File\n");

    int method;
    scanf("%d", &method);
    clear_buffer();
    
    if (method == 1) 
    {
        get_user_input(expected);
    }
     else 
     {
        printf("Enter expected output file path: ");
        char output_file[256];
        fgets(output_file, sizeof(output_file), stdin);
        output_file[strcspn(output_file, "\n")] = '\0';
        read_file(output_file, expected);
    }
}

void save_input_to_file(char *input) 
{
    FILE *temp = fopen("my_input.txt", "w");

    if (temp == NULL) 
    {
        printf("ERROR: Cannot create my_input.txt file!\n");
        printf("Check directory permissions.\n");
        return;
    }
    fprintf(temp, "%s", input);
    fclose(temp);
}


void run_program_and_capture_output(char *actual)
{
    //2-second timeout
    int exit_code = system("timeout 2s ./myprogram < my_input.txt > my_output.txt 2>&1");
    
    FILE *out = fopen("my_output.txt", "r");
    if (out != NULL) {
        actual[0] = '\0';
        char line[200];
        int first = 1;
        
        while (fgets(line, sizeof(line), out) != NULL) 
        {
            if (!first)
            {
                strcat(actual, "\n");
            }
            line[strcspn(line, "\n")] = '\0';
            strcat(actual, line);
            first = 0;
        }
        fclose(out);
        
        //timeout
        if (WIFEXITED(exit_code)) 
        {
            int status = WEXITSTATUS(exit_code);
            if (status == 124) 
            {
                strcat(actual, "\n[TIMEOUT: Program took too long (possible infinite loop)]");
            } else if (status != 0) 
            {
                char error_msg[100];
                sprintf(error_msg, "\n[PROGRAM EXITED WITH CODE %d]", status);
                strcat(actual, error_msg);
            }
        } 
        else if (WIFSIGNALED(exit_code)) 
        {
            int signal_num = WTERMSIG(exit_code);
            char error_msg[100];
            
            if (signal_num == SIGSEGV)
            {
                sprintf(error_msg, "\n[RUNTIME ERROR: Segmentation fault]");
            }
             else if (signal_num == SIGFPE)
             {
                sprintf(error_msg, "\n[RUNTIME ERROR: Floating point exception]");
            }
             else if (signal_num == SIGABRT)
             {
                sprintf(error_msg, "\n[RUNTIME ERROR: Program aborted]");
            }
             else 
            {
                sprintf(error_msg, "\n[RUNTIME ERROR: Terminated by signal %d]", signal_num);
            }
            strcat(actual, error_msg);
        }
    } 
    else 
    {
        strcpy(actual, "NO OUTPUT");
    }
}


void display_comparison(char *input, char *expected, char *actual) 
{
    printf("\nComparison\n");
    printf("Input given:\n%s\n", input);
    printf("Expected output:\n%s\n", expected);
    printf("Actual output:\n%s\n", actual);
}

int execute_test_case(int test_number) 
{
    printf("\n          Test Case %d    \n", test_number);
    
    char input[LIM], expected[LIM], actual[LIM];
    
    get_input_data(input);
    get_expected_output(expected);
    
    // SAVE INPUT
    save_input_to_file(input);
    
    printf("\nRunning program\n");
    run_program_and_capture_output(actual);
    
    display_comparison(input, expected, actual);
    
    // Check for timeout or runtime errors
    if (strstr(actual, "[TIMEOUT") != NULL) 
    {
        printf(" TIMEOUT - Possible infinite loop\n");
        return 0;
    }
    
    if (strstr(actual, "[RUNTIME ERROR") != NULL || 
        strstr(actual, "[PROGRAM EXITED") != NULL) 
        {
        printf(" RUNTIME ERROR\n");
        return 0;
    }
    
    if (compare_outputs(expected, actual)) 
    {
    printf(" PASS\n");
    return 1;
} 
else 
{
    printf(" FAIL\n");
    return 0;
}
}

void cleanup_test_files() 
{
    remove("my_input.txt");
    remove("my_output.txt");
}

void display_final_result(int passed, int total) 
{
    printf("\nFINAL RESULT\n");
    printf("Passed: %d/%d\n", passed, total);
}

