#include<stdio.h>

struct number
{
	int num1;
	int num2;
	char oper[5];
	//int (*ptr)(int, int);
};

int add(int a, int b)
{
	return a+b;
}

int sub(int a, int b)
{
	return a-b;
}

int mul(int a, int b)
{
	return a*b;
}

int div(int a, int b)
{
	if(b != 0)
	{
		return a/b;
	}
	return 0;
}


int main()
{
	FILE *fp;
	struct number nums;
	int result;
	char str[50];
	int num1=0, num2=0, i=0;	

	printf("Enter two numbers along with operation\n");
	scanf("%[^\n]", str);
	
	while(str[i] != '\0')
	{
		while(str[i] != ' ')
		{
			num1 = (num1 * 10) + (str[i] - '0');
			i++;
		}
		i++;
		while(str[i] != ' ')
		{
			num2 = (num2 * 10) + (str[i] - '0');
			i++;
		}
		i++;
		
	fp = fopen("/dev/dummy_char", "r+");
	fwrite(&nums, sizeof(struct number), 1, fp);
	
	fflush(fp);
	rewind(fp);

	fread(&result, sizeof(int), 1, fp);

	printf("sum is %d\n", result);

	fclose(fp);

	return 0;
}
