
#include<stdio.h>
#include<stdlib.h>

int main()
{
    int arr[2]={10,20};
    int res;
    FILE *fp= fopen("/dev/add_kernel","r+");
    if(fp == NULL)
    {
        /* code */
        perror("file open failed");
        return 1;
    }
    fwrite(arr, sizeof(int),2,fp);
    fread(&res,sizeof(res),1,fp);

    printf("sum %d ",res);
    fclose(fp);
    
}