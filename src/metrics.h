#ifndef METRICS
#define METRICS

typedef struct {
    int loc;               
    int lloc;             
    int comments;          
    int halstead_n1;       
    int halstead_n2;       
    int halstead_N1;       
    int halstead_N2;       
    double halstead_length; 
    double halstead_vocab;  
    double halstead_volume; 
    double halstead_difficulty; 
    double halstead_effort;     
    int functions;    
    
     
    int cyclomatic_complexity;
    int decision_points;
    
   
    int cognitive_complexity;
    int nesting_level;
    int max_nesting;
    
} CodeMetrics;


int analyze_source_file(const char *filename, CodeMetrics *metrics);


void display_metrics(const CodeMetrics *metrics);

int calculate_cyclomatic_complexity(const char *filename, CodeMetrics *metrics) ;
int calculate_cognitive_complexity(const char *filename, CodeMetrics *metrics);

#endif