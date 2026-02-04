

//ED

int calculate_edit_distance(const char *str1, const char *str2) 
{
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    
    
    int **dp = (int **)malloc((len1 + 1) * sizeof(int *));
    for (int i = 0; i <= len1; i++) 
    {
        dp[i] = (int *)malloc((len2 + 1) * sizeof(int));
    }
    
   
    for (int i = 0; i <= len1; i++)
    
    dp[i][0] = i;
    for (int j = 0; j <= len2; j++) 
    dp[0][j] = j;
    
  
    for (int i = 1; i <= len1; i++) 
    {
        for (int j = 1; j <= len2; j++) 
        {
            if (str1[i-1] == str2[j-1]) 
            {
                dp[i][j] = dp[i-1][j-1];
            }
             else 
             {
                int insert = dp[i][j-1] + 1;
                int delete = dp[i-1][j] + 1;
                int replace = dp[i-1][j-1] + 1;
                
                dp[i][j] = (insert < delete) ? insert : delete;
                dp[i][j] = (dp[i][j] < replace) ? dp[i][j] : replace;
            }
        }
    }

     int result = dp[len1][len2];
    
     for (int i = 0; i <= len1; i++) {
        free(dp[i]);
    }
    free(dp);
    
    return result;
}