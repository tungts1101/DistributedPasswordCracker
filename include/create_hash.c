#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "config.h"

void main(int argc, char **argv) {
	if(argc < 2)
		return;

	if(strlen(argv[1]) > PW_LENGTH) {
		return;
	}

	char password[PW_LENGTH];
	strcpy(password, argv[1]);

	printf("password = %s\n", password);
	printf("%s\n", crypt(password, SALT));
	
	return;
}