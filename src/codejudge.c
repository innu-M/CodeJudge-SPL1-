#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIM 2000



void clear_buffer() 
{
    int c;
    while((c = getchar()) != '\n' && (c != EOF));

}

void read_file(char *filename, char *content) 
{
    FILE *file = fopen(filename,"r");

    if(file == NULL) 
    {
        printf("File not found: %s\n", filename);
        strcpy(content, "");
        return;
    }
    
    content[0] = '\0';
    char line[200];
    int first = 1;
    
    while(fgets(line, sizeof(line),file) != NULL) 
    {
        if(!first) 
        {
            strcat(content, "\n");
        }
        line[strcspn(line, "\n")] = '\0';
        strcat(content, line);
        first = 0;
    }
    
    fclose(file);
}

int main() 
{
    char content[LIM];
    char filename[256];

    printf("Enter file name:\n");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';

    read_file(filename, content);
    //Does works or not
    printf("\nFile content:\n");
    printf("%s\n", content);

    /*
    system("cd");
    system("gcc \"D:\\\\Desktop\\\\CP\\\\demo3.c\" -o \"D:\\\\Desktop\\\\CP\\\\demo3.exe\"");

    system("\"D:\\\\Desktop\\\\CP\\\\demo3.exe\"");*/

    return 0;
}
