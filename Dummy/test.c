#include<stdio.h>

struct number
{
	int num1;
	int num2;
};

int main()
{
	FILE *fp;
	struct number nums;
	int result;	

	printf("Enter num1 and num2\n");
	scanf("%d%d", &nums.num1, &nums.num2);

	fp = fopen("/dev/dummy_char", "r+");
	fwrite(&nums, sizeof(struct number), 1, fp);
	
	fflush(fp);
	rewind(fp);

	fread(&result, sizeof(int), 1, fp);

	printf("sum is %d\n", result);

	fclose(fp);

	return 0;
}
