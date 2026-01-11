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
    int cyclomatic;        
    int functions;         
} CodeMetrics;


int analyze_source_file(const char *filename, CodeMetrics *metrics);


void display_metrics(const CodeMetrics *metrics);

#endif