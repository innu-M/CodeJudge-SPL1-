#include "Testing.h"



int main() 
{
    display_header();
    
    printf("Enter C file path: ");
    char *source_file = read_string();
    
    if (!source_file) 
    {
        printf("Error!! Could not read file path\n");
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