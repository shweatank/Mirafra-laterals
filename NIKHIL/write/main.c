// #include <unistd.h>
// #include <stdio.h>

// int main()
// {
//     int x = 123;
//     char buffer[20];
//     int len = sprintf(buffer, "%d", x);
//     write(1, buffer, len);
//     return 0;
// }


#include<unistd.h>

int main()
{
    char b[]="123";
    write(1,b,3);
}