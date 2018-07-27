
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *args[])
{
	printf("SUCCESS\n");
	system("useradd -d /home/abc abc -m");
	system("echo abc:123456 | chpasswd");

	return 0;
}