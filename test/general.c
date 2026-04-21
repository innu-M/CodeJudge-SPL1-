#include<stdio.h>

int main()
{
    //bello
    
    int n,sum=0;
    scanf("%d", &n);
    for(int i=1;i<=n;i++)
    {
        sum=sum+i;
    }
    printf("you got %d and nothing\n", sum);
    return 0;
}