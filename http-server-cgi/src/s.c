
#include <stdio.h>
#include <string.h>


int main(int argc, char *args[])
{
	if (argc == 1)
	{
		FILE *p = fopen("default.html", "r");
		if (p)
		{
			while (!feof(p))
			{
				char buf[1024] = { 0 };
				fgets(buf, sizeof(buf), p);
				printf("%s\n", buf);
			}
			fclose(p);
		}

	}

	if (argc == 2)
	{
		FILE *p = fopen("templet.zhujy", "r");
		char value[100] = { 0 };
		sscanf(args[1], "wd=%s", value);
		if (p)
		{
			char buf[1024] = { 0 };
			fread(buf, sizeof(buf), 1, p);
			int is_found = 0;
			char body[1024] = { 0 };
			char tmp[1024] = { 0 };
			FILE *p1 = fopen("data.txt", "r");
			if (p1)
			{
				while (!feof(p1))
				{
					memset(tmp, 0, sizeof(tmp));
					fgets(tmp, sizeof(tmp), p1);
					if (strstr(tmp, value))
					{
						is_found = 1;
						strcat(body, tmp);
					}
				}
				fclose(p1);
			}

			if (is_found)
				printf(buf, value, body);
			else
				printf(buf, value, "<a href=http://www.itcast.cn>www.itcast.cn</a>");
			fclose(p);
		}

	}

	return 0;
}