#ifndef Testing_H
#define Testing_H



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

extern FileSession sessions[100];
extern int session_count;
extern int current_session;


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


int new_session(const char *source_file);

#endif