#include<stdio.h>
int add(int a,int b){return a+b;}
int sub(int a,int b){return a-b;}
int mul(int a,int b){return a*b;}
int div(int a,int b){return a/b;}
int (*fnptr[])(int,int)={add,sub,mul,div};
int main()
{
int opt,a=5,b=5;
    
printf("enter the\n0 for add\n1 for sub\n2 for mul\n3 for div");
scanf("%d",&opt);
int c = fnptr[opt](a,b);
printf("\n%d\n",c);
}



