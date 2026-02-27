#include<stdio.h>
#include<stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 16

struct student_t{
    char student_name[BUF_SIZE];
    int year;
    char branch[BUF_SIZE];
};

int main(int argc, char* argv[]){
    struct student_t student1;
    FILE* fd = fopen("source.txt", "r+");
    if(fd == NULL){
        perror("open failed\n");
    }


    fscanf(fd, "%s %d %s",student1.student_name,&student1.year,student1.branch);

    

    printf("%s\n", student1.student_name);
    printf("%d\n", student1.year);
    printf("%s\n", student1.branch);

    fclose(fd);
    return 0;
}