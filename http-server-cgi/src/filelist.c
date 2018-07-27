
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char *args[])
{

	DIR *dp;
	struct dirent *dirp;
	dp = opendir("./mv");
	if (dp == NULL)
	{
		//printf("");
		return 0;
	}
	
	char buf[2048] = { 0 };
	while((dirp = readdir(dp)) != NULL)
	{
		char tmp[100] = { 0 };
		if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0)
		sprintf(tmp, "%s\n", dirp->d_name);
		strcat(buf, tmp);
	}
	
	closedir(dp);
	printf("%s\n", buf);
	return 0;
}
