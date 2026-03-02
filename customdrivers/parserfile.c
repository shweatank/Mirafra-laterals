#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LINE_SIZE 256

void readfile(const char *path){
    FILE *fp = fopen(path,"r");
    printf("Opening file: %s\n", path);
    if(fp==NULL){
        perror("file1");
        return;
    }

    char line[LINE_SIZE];
    char name[50]="";
    char branch[50]="";
    int year = 0;

    while(fgets(line,sizeof(line),fp)){

        if (strncmp(line,"name",4)==0)
            sscanf(line,"%*[^:]: %[^\n]", name);

        if (strncmp(line,"year",4)==0)
            sscanf(line,"%*[^:]: %d", &year);

        if (strncmp(line,"branch",6)==0)
            sscanf(line,"%*[^:]: %[^\n]", branch);
    }

    fclose(fp);

    printf("\nstudent data----->\n");
    printf("Name   : %s\n",name);
    printf("Year   : %d\n",year);
    printf("Branch : %s\n",branch);
}
void getfilepath(char *path){
    FILE *fp = fopen("file2.txt", "r+");    
    if(fp==NULL){
        perror("no file found\n");
        exit(1);
    }

    char line[LINE_SIZE];
    while(fgets(line,sizeof(line),fp)){
        sscanf(line,"path:%s",path);
    }
    fclose(fp);
}
int main(){
    char file1_path[100];
    while(1){
        getfilepath(file1_path);
        readfile(file1_path);

        sleep(3);
    }
    return 0;
}