#include<stdio.h>
#include<stdlib.h>

struct data
{
    char name[50];
    int year;
    char branch[50];
};

int main()
{
    struct data s;
    FILE *fd, *fd1;
    char filepath[200];

    fd = fopen("/home/mirafra/test/GIT_MIRAFRA/Mirafra/NIKHIL/struct_files/file1.txt","r");

    if (fd == NULL)
    {
        printf("Not opening\n");
        return 1;
    }
    fscanf(fd, "%s", filepath);
    fclose(fd);
    
    fd1= fopen(filepath, "r");

    while (fscanf(fd1, "%s %d %s", s.name, &s.year, s.branch) != EOF)
    {
        printf("Name   : %s\n", s.name);
        printf("Year   : %d\n", s.year);
        printf("Branch : %s\n", s.branch);
        printf("-----------------\n");
    }

    fclose(fd1);
    return 0;
}