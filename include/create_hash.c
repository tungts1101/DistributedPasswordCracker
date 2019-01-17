#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void main(int argc, char **argv) {
	if(argc < 2)
		return;

	char SALT[2] = "aa";
	char *password = malloc(100);
	memset(password, 0, 100);
	strcpy(password, argv[1]);
	password[strlen(password) - 1] = '\0';

	printf("password = %s\n", password);
	printf("%s\n", crypt(password, SALT));
	
	return;
}