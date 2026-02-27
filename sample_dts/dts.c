#include<stdio.h>
#include<string.h>

int main()
{
	FILE *fp = fopen("file1.txt", "r+");

	if(fp == NULL)
	{
		perror("fopen");
		return 1;
	}

	char filename[30];
	char path[100];

	if (fgets(filename, sizeof(filename), fp)) 
	{
		filename[strcspn(filename, "\n")] = '\0';
	}

	if (fgets(path, sizeof(path), fp)) 
	{
		path[strcspn(path, "\n")] = '\0';
	}

	strcat(path, filename);

	printf("path %s\n", path);

	FILE *data = fopen(path, "r+");

	char name[40];
	char year[30];
	char branch[30];

	if (fgets(name, sizeof(name), data))
        {
                name[strcspn(name, "\n")] = '\0';
        }

	if (fgets(year, sizeof(year), data))
        {
                year[strcspn(year, "\n")] = '\0';
        }

	if (fgets(branch, sizeof(branch), data))
        {
                branch[strcspn(branch, "\n")] = '\0';
        }

	printf("Name: %s\n", name);
	printf("year: %s\n", year);
	printf("branch: %s\n", branch);

	return 0;
}
