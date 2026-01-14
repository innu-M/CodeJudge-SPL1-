#include "mathlib.h"

double log(double x) 
{
    if (x <= 0) return -1;
    if (x == 1) return 0;
    
   
    int adjustments = 0;
    while (x > 2.0) 
    {
        x /= 2.718281828;
        adjustments++;
    }
    
    
    double y = x - 1;
    double result = 0;
    double term = y;
    
    for (int n = 1; n <= 100; n++) 
    {
        result += term / n;
        term *= -y;
    }
    
    return result + adjustments;
}
