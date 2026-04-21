#include<sys/wait.h>
#include<signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "metrics.h"

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define CYAN "\033[1;36m"
#define YELLOW "\033[1;33m" 
#define BLUE "\033[1;34m"    
#define BOLD "\033[1m"       
#define RESET "\033[0m"

typedef struct {
    char *data;
    size_t size;
    size_t capacity;
} DBuffer;

typedef struct {
    int test_number;
    int passed;
    char *input;
    char *expected;
    char *actual;
    char *error_msg;
} TestResult;


typedef struct {
    char *source_file;
    char binary_name[64];
    char input_file[64];
    char output_file[64];
    TestResult *test_results;
    int total_tests;
    int passed;
} FileSession;

FileSession sessions[100];
int session_count  = 0;
int current_session = -1;


void clear_buffer();
DBuffer* create_buffer();
void free_buffer(DBuffer *buffer);
void append_to_buffer(DBuffer *buffer, const char *str);
int  read_file(const char *filename, DBuffer *buffer);
void get_user_input(DBuffer *buffer);
char* read_string();
int  compile_program(const char *source_file, const char *binary_name);
void get_input_data(DBuffer *buffer);
void get_expected_output(DBuffer *buffer);
void save_input_to_file(const char *input, const char *input_file);
void run_program_and_CtO(DBuffer *buffer, const char *binary, const char *input_file, const char *output_file);
void comparison(const char *input, const char *expected, const char *actual);
int  execute_test_case(int test_number, FileSession *s);
void cleanup_test_files(const char *input_file, const char *output_file);
void display_result(int passed, int total, TestResult *test_results);
void code_metrics(const char *source_file);
void display_menu();
void display_header();


int new_session(const char *source_file)
{
    if (session_count >= 16) {
        printf("Max files reached\n");
        return 0;
    }
    int idx = session_count++;
    FileSession *s = &sessions[idx];
    s->source_file  = strdup(source_file);
    s->test_results = NULL;
    s->total_tests  = 0;
    s->passed       = 0;
    snprintf(s->binary_name, sizeof(s->binary_name), "myprogram_%d", idx);
    snprintf(s->input_file,  sizeof(s->input_file),  "my_input_%d.txt",  idx);
    snprintf(s->output_file, sizeof(s->output_file), "my_output_%d.txt", idx);
    current_session = idx;
    return 1;
}

int main() 
{
    display_header();
    
    printf("Enter C file path: ");
    char *source_file = read_string();
    
    if (!source_file) 
    {
        printf("Error: Could not read file path\n");
        return 1;
    }

    if (!new_session(source_file)) 
    {
        free(source_file);
        return 1;
    }
    free(source_file); 

    FileSession *s = &sessions[current_session];
    if (!compile_program(s->source_file, s->binary_name)) 
        return 1;
    
    int choice;
    CodeMetrics metrics = {0};

    do 
    {
        
        s = &sessions[current_session];

        display_menu();
        printf("Enter choice: ");
        
        if (scanf("%d", &choice) != 1) 
        {
            clear_buffer();
            printf("Invalid input\n");
            continue;
        }
        clear_buffer();
        
        switch(choice) 
        {
            case 1: {
                s->total_tests++;
                printf("\n------------------ Test Case %d -----------------\n", s->total_tests);
                
                TestResult *temp = realloc(s->test_results, s->total_tests * sizeof(TestResult));
                if (!temp) 
                {
                    printf("Memory allocation failed\n");
                    s->total_tests--;
                    break;
                }
                s->test_results = temp;
                
                s->passed += execute_test_case(s->total_tests, s);
                cleanup_test_files(s->input_file, s->output_file);
                break;
            }
                
            case 2:
                if (s->total_tests > 0) 
                    display_result(s->passed, s->total_tests, s->test_results);

                else 
                    printf("No test cases run yet\n");
                break;
                
            case 3:
                code_metrics(s->source_file);
                break;
                
            case 4:
                printf("\n---------------------ADVANCED COMPLEXITY ANALYSIS---------------------\n");
                calculate_cyclomatic_complexity(s->source_file, &metrics);
                calculate_cognitive_complexity(s->source_file, &metrics);
                
                printf("\nCyclomatic Complexity: %d\n", metrics.cyclomatic_complexity);
                printf("Decision Points: %d\n", metrics.decision_points);
                printf("\nInterpretation:\n");



                //recommend
                if (metrics.cyclomatic_complexity <= 10)
                    printf(GREEN "  Low complexity - Easy to test and maintain\n" RESET);

                else if (metrics.cyclomatic_complexity <= 20)
                    printf(YELLOW " Moderate complexity - Consider refactoring\n" RESET);


                else
                    printf(RED "  High complexity - Refactoring recommended\n" RESET);
                
                printf("\nCognitive Complexity: %d\n", metrics.cognitive_complexity);


                printf("Maximum Nesting Level: %d\n", metrics.max_nesting);
                printf("\nInterpretation:\n");


                if (metrics.cognitive_complexity <= 15)
                    printf(GREEN "Easy to understand\n" RESET);
                
                    else if (metrics.cognitive_complexity <= 30)
                    printf(YELLOW " Moderately complex - May need simplification\n" RESET);
                else
                    printf(RED "Difficult to understand - Simplification needed\n" RESET);
                break;

            case 5:
                printf("\n---------------------BUFFER OVERFLOW RISK ANALYSIS---------------------\n");
                detect_buffer_overflow_risk(s->source_file, &metrics);
                
                printf("Risk Score: %d\n", metrics.buffer_overflow_risk);
                printf("Unsafe Functions Found: %d\n", metrics.unsafe_functions_count);
                printf("Fixed-size Buffers: %d\n\n", metrics.fixed_buffer_count);
                
                printf("\nOverall Risk Level: ");
                if (metrics.buffer_overflow_risk < 10)
                    printf(GREEN "LOW\n" RESET);


                else if (metrics.buffer_overflow_risk < 30)
                    printf(YELLOW "MEDIUM\n" RESET);

                else
                    printf(RED "HIGH!\n" RESET);
                break;

            
            case 6: {
                printf("Enter new C file path: ");
                char *new_file = read_string();
                if (new_file && new_session(new_file)) {
                    FileSession *ns = &sessions[current_session];
                    if (!compile_program(ns->source_file, ns->binary_name)) 
                    {
                        
                        free(ns->source_file);
                        session_count--;
                        current_session = session_count - 1;
                        printf("Staying on previous file.\n");
                    }
                }
                free(new_file);
                break;
            }

            
            case 7: {
                if (session_count == 1) {
                    printf("Only one file loaded.\n");
                    break;
                }
                printf("Loaded files:\n");
                for (int i = 0; i < session_count; i++)
                   if (i == current_session) {
                    printf("  %d. %s%s\n", i + 1, sessions[i].source_file, " [active]");
                      }
                            
                     else 
                    {
                      printf("  %d. %s%s\n", i + 1, sessions[i].source_file, "");
                    }

                printf("Switch to (number): ");
                int pick;
                scanf("%d", &pick);
                clear_buffer();
                if (pick >= 1 && pick <= session_count)
                    current_session = pick - 1;
                else
                    printf("Invalid choice\n");
                break;
            }

            case 8:
                printf("Exiting...\n");
                break;
                
            default:
                printf("Invalid choice\n");
        }
        
        if (choice != 8) 
        {
            printf("\nPress Enter to continue...");
            clear_buffer();
        }
        
    } while (choice != 8);
    
  
    for (int i = 0; i < session_count; i++) 
    {
        for (int j = 0; j < sessions[i].total_tests; j++) 
        {
            free(sessions[i].test_results[j].input);
            free(sessions[i].test_results[j].expected);
            free(sessions[i].test_results[j].actual);
            free(sessions[i].test_results[j].error_msg);
        }
        free(sessions[i].test_results);
        free(sessions[i].source_file);
        remove(sessions[i].binary_name);
    }
    
    return 0;
}

void display_header() 
{
    printf("\033[1m\033\e[1;36m");

    printf("\n\n\n");

    printf("   ██████╗ ██████╗ ██████╗ ███████╗     ██╗██╗   ██╗██████╗  ██████╗ ███████╗\n");
    printf("  ██╔════╝██╔═══██╗██╔══██╗██╔════╝     ██║██║   ██║██╔══██╗██╔════╝ ██╔════╝\n");
    printf("  ██║     ██║   ██║██║  ██║█████╗       ██║██║   ██║██║  ██║██║  ███╗█████╗  \n");
    printf("  ██║     ██║   ██║██║  ██║██╔══╝  ██   ██║██║   ██║██║  ██║██║   ██║██╔══╝  \n");
    printf("  ╚██████╗╚██████╔╝██████╔╝███████╗╚█████╔╝╚██████╔╝██████╔╝╚██████╔╝███████╗\n");
    printf("   ╚═════╝ ╚═════╝ ╚═════╝ ╚══════╝ ╚════╝  ╚═════╝ ╚═════╝ ╚═════╝ ╚══════╝\n");
    printf("\n\n\n");
    printf("\033[0m");
       
}

void display_menu() 
{
    printf(CYAN "\n------------------------------------ MENU-----------------------------------\n" RESET);
    printf("                                     1. Run Test Case\n");
    printf("                                     2. View Test Results\n");
    printf("                                     3. Analyze Code Metrics\n");
    printf("                                     4. Advanced Metrics (Cyclomatic & Cognitive)\n");
    printf("                                     5. Buffer Overflow Risk Analysis\n");
    printf("                                     6. Add New File\n");
    printf("                                     7. Switch File\n");
    printf("                                     8. Exit\n");
    printf(CYAN "-------------------------------------------------------------------------------\n" RESET);
}

void clear_buffer() 
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

DBuffer* create_buffer() 
{
    DBuffer *buffer = malloc(sizeof(DBuffer));
    if (!buffer) return NULL;
    
    buffer->capacity = 1024;
    buffer->size = 0;
    buffer->data = malloc(buffer->capacity);
    
    if (!buffer->data) 
    {
        free(buffer);
        return NULL;
    }
    
    buffer->data[0] = '\0';
    return buffer;
}

void free_buffer(DBuffer *buffer) 
{
    if (buffer) 
    {
        free(buffer->data);
        free(buffer);
    }
}

void append_to_buffer(DBuffer *buffer, const char *str) 
{
    if (!buffer || !str) return;
    
    size_t str_len = strlen(str);
    
    while (buffer->size + str_len + 1 >= buffer->capacity) 
    {
        size_t new_capacity = buffer->capacity * 2;
        char *new_data = realloc(buffer->data, new_capacity);
        
        if (!new_data) 
        {
            fprintf(stderr, "Memory allocation failed\n");
            return;
        }
        
        buffer->data = new_data;
        buffer->capacity = new_capacity;
    }
    
    strcat(buffer->data, str);
    buffer->size += str_len;
}

char* read_string() 
{
    size_t capacity = 256;
    size_t size = 0;
    char *str = malloc(capacity);
    
    if (!str) return NULL;
    
    int c;
    while ((c = getchar()) != '\n' && c != EOF) 
    {
        str[size++] = c;
        
        if (size >= capacity - 1) 
        {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) 
            {
                free(str);
                return NULL;
            }
            str = new_str;
        }
    }
    
    str[size] = '\0';
    return str;
}

int read_file(const char *filename, DBuffer *buffer) 
{
    FILE *file = fopen(filename, "r");
    
    if (file == NULL) 
    {
        printf(RED "File not found: %s\n" RESET, filename);
        return 0;
    }
    
    char line[512];
    int first = 1;
    
    while (fgets(line, sizeof(line), file) != NULL) 
    {
        if (!first) 
            append_to_buffer(buffer, "\n");
        
        line[strcspn(line, "\n")] = '\0';
        line[strcspn(line, "\r")] = '\0';
        append_to_buffer(buffer, line);
        first = 0;
    }
    
    fclose(file);
    return 1;
}

void get_user_input(DBuffer *buffer) 
{
    char line[512];
    int line_count = 0;
    
    printf("Enter input (press Enter twice to finish):\n");
    
    while (1) 
    {
        if (fgets(line, sizeof(line), stdin) == NULL) 
            break;
        
        line[strcspn(line, "\n")] = '\0';
        
        if (line[0] == '\0' && line_count > 0) 
            break;
        
        if (line_count > 0) 
            append_to_buffer(buffer, "\n");
        
        append_to_buffer(buffer, line);
        line_count++;
    }
}

int compile_program(const char *source_file, const char *binary_name) 
{
    size_t cmd_size = strlen(source_file) + strlen(binary_name) + 100;
    char *command = malloc(cmd_size);
    
    if (!command) 
    {
        printf("Memory allocation failed\n");
        return 0;
    }
    
   
    snprintf(command, cmd_size, "gcc %s -o %s -lm 2> error.txt", source_file, binary_name);
    
    int compile_result = system(command);
    free(command);

    FILE *error_file = fopen("error.txt", "r");
    if (error_file != NULL) 
    {
        char ch;
        int has_error = 0;
        //printf("Compilation output:\n");
        
        while ((ch = fgetc(error_file)) != EOF) 
        {
            putchar(ch);
            has_error = 1;
        }
        fclose(error_file);
        
        if (has_error) 
        {
            printf("Compilation FAILED\n");
            remove("error.txt");
            return 0;
        }
    }
    
    remove("error.txt");
    // printf("Compilation SUCCESS\n");
    return 1;
}

void get_input_data(DBuffer *buffer) 
{
    printf("\nInput from:\n");
    printf(" 1. Keyboard\n");
    printf(" 2. File\n");
    printf("Choice: ");
    
    int method;
    scanf("%d", &method);
    clear_buffer();
    
    if (method == 1) 
    {
        get_user_input(buffer);
    } 
    else
    {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)))
            printf("Current folder: %s\n", cwd);

        while (1) {
            printf("Input File(path): ");
            char *input_file = read_string();
            if (!input_file) break;

            if (read_file(input_file, buffer)) {
                free(input_file);
                break;
            }

            printf(CYAN "Help: if your file is inside the 'test' folder, type: test/<filename>\n" RESET);
            free(input_file);
        }
    }
}

void get_expected_output(DBuffer *buffer) 
{
    printf("\nExpected output from:\n");
    printf(" 1. Keyboard\n");
    printf(" 2. File\n");
    printf("Choice: ");
    
    int method;
    scanf("%d", &method);
    clear_buffer();
    
    if (method == 1) 
    {
        get_user_input(buffer);
    } 
    else
    {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)))
            printf("Current folder: %s\n", cwd);

        while (1) {
            printf("Expected Output File(Path): ");
            char *output_file = read_string();
            if (!output_file) break;

            if (read_file(output_file, buffer)) {
                free(output_file);
                break;
            }

            printf(CYAN "Help: if your file is inside the 'test' folder, type: test/<filename>\n" RESET);
            free(output_file);
        }
    }
}


void save_input_to_file(const char *input, const char *input_file) 
{
    FILE *temp = fopen(input_file, "w");
    
    if (temp == NULL) 
    {
        printf("ERROR: Can't create input file\n");
        return;
    }
    
    fprintf(temp, "%s", input);
    fclose(temp);
}


void run_program_and_CtO(DBuffer *buffer, const char *binary, const char *input_file, const char *output_file)
{
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "timeout 10s ./%s < %s > %s 2>&1", binary, input_file, output_file);
    int exit_code = system(cmd);

    FILE *out = fopen(output_file, "r");
    if (out != NULL) 
    {
        char line[512];
        int first = 1;
        
        while (fgets(line, sizeof(line), out) != NULL) 
        {
            if (!first)
                append_to_buffer(buffer, "\n");
            
            line[strcspn(line, "\n")] = '\0';
            line[strcspn(line, "\r")] = '\0';
            append_to_buffer(buffer, line);
            first = 0;
        }
        fclose(out);
        
        if (WIFEXITED(exit_code)) 
        {
            int status = WEXITSTATUS(exit_code);
            if (status == 124) 
            {
                append_to_buffer(buffer, "\n[TIMEOUT: Program exceeded 10 seconds]");
            } 
            else if (status != 0) 
            {
                char error_msg[100];
                snprintf(error_msg, sizeof(error_msg), 
                        "\n[PROGRAM EXITED WITH CODE %d]", status);
                append_to_buffer(buffer, error_msg);
            }
        } 
        else if (WIFSIGNALED(exit_code)) 
        {
            int signal_num = WTERMSIG(exit_code);
            char error_msg[100];
            
            if (signal_num == SIGSEGV)
                snprintf(error_msg, sizeof(error_msg), "\n[RUNTIME ERROR: Segmentation fault]");

            else if (signal_num == SIGFPE)
                snprintf(error_msg, sizeof(error_msg), "\n[RUNTIME ERROR: Floating point exception]");

            else if (signal_num == SIGABRT)
                snprintf(error_msg, sizeof(error_msg), "\n[RUNTIME ERROR: Program aborted]");
            else
                snprintf(error_msg, sizeof(error_msg), "\n[RUNTIME ERROR: Signal %d]", signal_num);

            append_to_buffer(buffer, error_msg);
        }
    } 
    else 
    {
        append_to_buffer(buffer, "[NO OUTPUT]");
    }
}

void comparison(const char *input, const char *expected, const char *actual) 
{
    printf("\n------------------------------------------------------------------\n");
    printf("|                           COMPARISON                              | \n");
    printf("--------------------------------------------------------------------\n");
    printf("Expected Output:\n%s\n\n", expected);
    printf("Actual Output:\n%s\n", actual);
    
    printf("Do you want to see the difference visualization? (y/n): ");
    char choice;
    scanf("%c", &choice);
    clear_buffer();
    
    if (choice == 'y' || choice == 'Y') 
    {
        if (strcmp(expected, actual) != 0) 
            visualize_output_difference(expected, actual);
        else 
            printf(GREEN "\nOutputs are identical, no differences to show.\n" RESET);
    }
}


int execute_test_case(int test_number, FileSession *s) 
{
    DBuffer *input    = create_buffer();
    DBuffer *expected = create_buffer();
    DBuffer *actual   = create_buffer();
    
    if (!input || !expected || !actual) 
    {
        printf("Memory allocation failed\n");
        free_buffer(input);
        free_buffer(expected);
        free_buffer(actual);
        return 0;
    }
    
    get_input_data(input);
    get_expected_output(expected);
    save_input_to_file(input->data, s->input_file);
    
    printf("\nCode is Running ..\n");
    run_program_and_CtO(actual, s->binary_name, s->input_file, s->output_file);
    
    comparison(input->data, expected->data, actual->data);
    
    int idx = test_number - 1;
    s->test_results[idx].test_number = test_number;
    s->test_results[idx].input       = strdup(input->data);
    s->test_results[idx].expected    = strdup(expected->data);
    s->test_results[idx].actual      = strdup(actual->data);
    s->test_results[idx].error_msg   = strdup("");
    
    int result = 0;
    
    if (strstr(actual->data, "[TIMEOUT") != NULL) 
    {
        printf(RED "\nTIMEOUT - Possible infinite loop\n" RESET);
        free(s->test_results[idx].error_msg);
        s->test_results[idx].error_msg = strdup("TIMEOUT");
        s->test_results[idx].passed = 0;
    } 
    else if (strstr(actual->data, "[RUNTIME ERROR") != NULL || 
             strstr(actual->data, "[PROGRAM EXITED") != NULL) 
    {
        printf(RED "\nRUNTIME ERROR\n" RESET);
        free(s->test_results[idx].error_msg);
        s->test_results[idx].error_msg = strdup("RUNTIME ERROR");
        s->test_results[idx].passed = 0;
    } 
    else if (strcmp(expected->data, actual->data) == 0)  
    {
        printf(GREEN "\nPASS\n" RESET);
        s->test_results[idx].passed = 1;
        result = 1;
    }
    else 
    {
        printf(RED "\nFAILED\n" RESET);
        free(s->test_results[idx].error_msg);
        s->test_results[idx].error_msg = strdup("Output mismatch");
        s->test_results[idx].passed = 0;
    }
    
    free_buffer(input);
    free_buffer(expected);
    free_buffer(actual);
    
    return result;
}


void cleanup_test_files(const char *input_file, const char *output_file) 
{
    remove(input_file);
    remove(output_file);
}


void display_result(int passed, int total, TestResult *test_results) 
{
    printf("\n--------------------------------------------------\n");
     printf("|                 FINAL RESULTS                   | \n");
    printf("--------------------------------------------------\n");
    printf("Total Tests:  %d\n", total);
    printf("Passed:       %d\n", passed);
    printf("Failed:       %d\n", total - passed);

    if (total > 0) 
    {
        double pass_percentage = (passed * 100.0) / total;
        printf("Pass Rate:    %.1f%%\n", pass_percentage);
    }

    printf("--------------------------------------------------\n");
    printf("\nDetailed Results:\n");

    for (int i = 0; i < total; i++) 
    {
       if (test_results[i].passed) {
    printf(" Test %d: %s", test_results[i].test_number, "PASS");
        }
        
    else 
    {
    printf(" Test %d: %s", test_results[i].test_number, "FAIL");
    }



        if (!test_results[i].passed && strlen(test_results[i].error_msg) > 0) 
            printf(" (%s)", test_results[i].error_msg);
        printf("\n");
    }
}

void code_metrics(const char *source_file)
{
    printf("\n----------------------- CODE METRICS ANALYSIS -----------------------\n");
   
    CodeMetrics metrics;
    if (analyze_source_file(source_file, &metrics)) 
        display_metrics(&metrics);
    else 
        printf("Failed to analyze metrics\n");
}