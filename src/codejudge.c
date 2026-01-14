#include<sys/wait.h>
#include<signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "metrics.h"

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define CYAN "\033[1;36m"
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

TestResult *test_results = NULL;
int total_tests = 0;

void clear_buffer();
DBuffer* create_buffer();
void free_buffer(DBuffer *buffer);
void append_to_buffer(DBuffer *buffer, const char *str);
void read_file(const char *filename, DBuffer *buffer);
void get_user_input(DBuffer *buffer);
char* read_string();
int compile_program(const char *source_file);
void get_input_data(DBuffer *buffer);
void get_expected_output(DBuffer *buffer);
void save_input_to_file(const char *input);
void run_program_and_CtO(DBuffer *buffer);
void comparison(const char *input, const char *expected, const char *actual);
int execute_test_case(int test_number);
void cleanup_test_files();
void display_result(int passed, int total);
void code_metrics(const char *source_file);
void display_menu();
void display_header();

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

    if (!compile_program(source_file)) 
    {
        free(source_file);
        return 1;
    }
    
    int passed = 0;
    int choice;
    
    do 
    {
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
            case 1:
                total_tests++;
                printf("\n========== Test Case %d ==========\n", total_tests);
                
                TestResult *temp = realloc(test_results, total_tests * sizeof(TestResult));
                if (!temp) 
                {
                    printf("Memory allocation failed\n");
                    break;
                }
                test_results = temp;
                
                passed += execute_test_case(total_tests);
                cleanup_test_files();
                break;
                
            case 2:
                if (total_tests > 0) 
                {
                    display_result(passed, total_tests);
                } 
                else 
                {
                    printf("No test cases run yet\n");
                }
                break;
                
           
            case 3:
                code_metrics(source_file);
                break;
                
            case 4:
                printf("\nExiting CodeJudge\n");
                break;
                
            default:
                printf("Invalid choice\n");
        }
        
        if (choice != 4) 
        {
            printf("\nPress Enter to continue...");
            clear_buffer();
        }
        
    } while (choice != 4);
    
    for (int i = 0; i < total_tests; i++) 
    {
        free(test_results[i].input);
        free(test_results[i].expected);
        free(test_results[i].actual);
        free(test_results[i].error_msg);
    }
    free(test_results);
    free(source_file);
    remove("myprogram");
    
    return 0;
}

void display_header() 
{
                    printf(CYAN "================================================\n" RESET);
                    printf(CYAN "                                                \n" RESET);
                    printf(CYAN "                   CODEJUDGE                    \n" RESET);
                    printf(CYAN "                                                \n" RESET);
                    printf(CYAN "================================================\n\n" RESET);
}

void display_menu() 
{
                    printf(CYAN "\n=================== MENU ===================\n" RESET);
                    printf("                1. Run Test Case\n");
                    printf("                2. View Test Results\n");
                    printf("                3. Analyze Code Metrics\n");
                    printf("                4. Exit\n");
                    printf(CYAN "============================================\n" RESET);
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

void read_file(const char *filename, DBuffer *buffer) 
{
    FILE *file = fopen(filename, "r");
    
    if (file == NULL) 
    {
        printf("File not found: %s\n", filename);
        return;
    }
    
    char line[512];
    int first = 1;
    
    while (fgets(line, sizeof(line), file) != NULL) 
    {
        if (!first) 
        {
            append_to_buffer(buffer, "\n");
        }
        
        line[strcspn(line, "\n")] = '\0';
        line[strcspn(line, "\r")] = '\0';
        append_to_buffer(buffer, line);
        first = 0;
    }
    
    fclose(file);
}

void get_user_input(DBuffer *buffer) 
{
    char line[512];
    int line_count = 0;
    
    printf("Enter input (press Enter twice to finish):\n");
    
    while (1) 
    {
        if (fgets(line, sizeof(line), stdin) == NULL) 
        {
            break;
        }
        
        line[strcspn(line, "\n")] = '\0';
        
        if (line[0] == '\0' && line_count > 0) 
        {
            break;
        }
        
        if (line_count > 0) 
        {
            append_to_buffer(buffer, "\n");
        }
        
        append_to_buffer(buffer, line);
        line_count++;
    }
}

int compile_program(const char *source_file) 
{
    size_t cmd_size = strlen(source_file) + 100;
    char *command = malloc(cmd_size);
    
    if (!command) 
    {
        printf("Memory allocation failed\n");
        return 0;
    }
    
    printf("Compiling %s...\n", source_file);
    snprintf(command, cmd_size, "gcc %s -o myprogram -lm 2> error.txt", source_file);
    
    int compile_result = system(command);
    free(command);

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
            printf("Compilation FAILED\n");
            remove("error.txt");
            return 0;
        }
    }
    
    remove("error.txt");
    printf("Compilation SUCCESS\n");
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
        printf("Input File(path): ");
        char *input_file = read_string();
        
        if (input_file) 
        {
            read_file(input_file, buffer);
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
        printf("Expected Output File(Path): ");
        char *output_file = read_string();
        
        if (output_file) 
        {
            read_file(output_file, buffer);
            free(output_file);
        }
    }
}

void save_input_to_file(const char *input) 
{
    FILE *temp = fopen("my_input.txt", "w");
    
    if (temp == NULL) 
    {
        printf("ERROR: Can't create input file\n");
        return;
    }
    
    fprintf(temp, "%s", input);
    fclose(temp);
}

void run_program_and_CtO(DBuffer *buffer)
{
    int exit_code = system("timeout 10s ./myprogram < my_input.txt > my_output.txt 2>&1");
    
    FILE *out = fopen("my_output.txt", "r");
    if (out != NULL) 
    {
        char line[512];
        int first = 1;
        
        while (fgets(line, sizeof(line), out) != NULL) 
        {
            if (!first)
            {
                append_to_buffer(buffer, "\n");
            }
            
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
                append_to_buffer(buffer, "\n[TIMEOUT: Program exceeded 2 seconds]");
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
            {
                snprintf(error_msg, sizeof(error_msg), 
                        "\n[RUNTIME ERROR: Segmentation fault]");
            }
            else if (signal_num == SIGFPE)
            {
                snprintf(error_msg, sizeof(error_msg), 
                        "\n[RUNTIME ERROR: Floating point exception]");
            }
            else if (signal_num == SIGABRT)
            {
                snprintf(error_msg, sizeof(error_msg), 
                        "\n[RUNTIME ERROR: Program aborted]");
            }
            else 
            {
                snprintf(error_msg, sizeof(error_msg), 
                        "\n[RUNTIME ERROR: Signal %d]", signal_num);
            }
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
    printf("\n========================================\n");
    printf("                COMPARISON                  \n");
    printf("========================================\n");
    printf("Input:\n%s\n\n", input);
    printf("Expected Output:\n%s\n\n", expected);
    printf("Actual Output:\n%s\n", actual);
    printf("========================================\n");
}

int execute_test_case(int test_number) 
{
    DBuffer *input = create_buffer();
    DBuffer *expected = create_buffer();
    DBuffer *actual = create_buffer();
    
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
    save_input_to_file(input->data);
    
    printf("\nCode is Running ..\n");
    run_program_and_CtO(actual);
    
    comparison(input->data, expected->data, actual->data);
    
    int idx = test_number - 1;
    test_results[idx].test_number = test_number;
    test_results[idx].input = strdup(input->data);
    test_results[idx].expected = strdup(expected->data);
    test_results[idx].actual = strdup(actual->data);
    test_results[idx].error_msg = strdup("");
    
    int result = 0;
    
    if (strstr(actual->data, "[TIMEOUT") != NULL) 
    {
        printf(RED "\nTIMEOUT - Possible infinite loop\n" RESET);
        free(test_results[idx].error_msg);
        test_results[idx].error_msg = strdup("TIMEOUT");
        test_results[idx].passed = 0;
    } 

    else if (strstr(actual->data, "[RUNTIME ERROR") != NULL || 
             strstr(actual->data, "[PROGRAM EXITED") != NULL) 
    {
        printf(RED "\nRUNTIME ERROR\n" RESET);
        free(test_results[idx].error_msg);
        test_results[idx].error_msg = strdup("RUNTIME ERROR");
        test_results[idx].passed = 0;
    } 

    else if (strcmp(expected->data, actual->data) == 0)  
    {
        printf(GREEN "\nPASS\n" RESET);
        test_results[idx].passed = 1;
        result = 1;
    }


    else 
    {
        printf(RED "\nFAILED\n" RESET);
        free(test_results[idx].error_msg);
        test_results[idx].error_msg = strdup("Output mismatch");
        test_results[idx].passed = 0;
    }
    
    free_buffer(input);
    free_buffer(expected);
    free_buffer(actual);
    
    return result;
}

void cleanup_test_files() 
{
    remove("my_input.txt");
    remove("my_output.txt");
}

void display_result(int passed, int total) 
{
printf("\n========================================\n");
printf("                FINAL RESULTS               \n");
printf("========================================\n");
printf("Total Tests:  %d\n", total);
printf("Passed:       %d\n", passed);
printf("Failed:       %d\n", total - passed);if (total > 0) 
{
    double pass_percentage = (passed * 100.0) / total;
    printf("Pass Rate:    %.1f%%\n", pass_percentage);
}

printf("========================================\n");

printf("\nDetailed Results:\n");
for (int i = 0; i < total; i++) 
{
    printf(" Test %d: %s", test_results[i].test_number, 
           test_results[i].passed ? "PASS" : "FAIL");
    if (!test_results[i].passed && strlen(test_results[i].error_msg) > 0) 
    {
        printf(" (%s)", test_results[i].error_msg);
    }
    printf("\n");
}

}


void code_metrics(const char *source_file)
{
                        printf("\n========================================\n");
                        printf("            ANALYZING CODE METRICS          \n");
                        printf("========================================    \n");
CodeMetrics metrics;
if (analyze_source_file(source_file, &metrics)) 
{
    display_metrics(&metrics);
} 

else 
{
    printf("Failed to analyze metrics\n");
}
}

