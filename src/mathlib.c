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

double my_pow(double base, int exp) 
{
    if (exp == 0) return 1.0;
    if (exp < 0) return 1.0 / my_pow(base, -exp);
    
    double result = 1.0;
    for (int i = 0; i < exp; i++) 
    {
        result *= base;
    }
    return result;
}

double my_sqrt(double x) 
{
    if (x < 0) return -1;
    if (x == 0) return 0;
    
    double guess = x / 2.0;
    double epsilon = 0.00001;
    
    while (my_abs(guess * guess - x) > epsilon) 
    {
        guess = (guess + x / guess) / 2.0;
    }
    
    return guess;
}

double my_abs(double x) 
{
    return x < 0 ? -x : x;
}