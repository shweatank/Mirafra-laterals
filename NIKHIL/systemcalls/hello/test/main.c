#include<unistd.h>

int main()
{
    char a[] = "helloworld";
    write(1,a,sizeof(a)-1);
  return 0;
}