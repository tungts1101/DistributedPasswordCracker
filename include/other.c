/* 2018/11/21
 * version 1.0
 * */

#include <stdlib.h>
#include <string.h>
#include <stdio.h> // printf()
#include "../lib/other.h"
#include "../lib/config.h"

char *getHash(char *other) {
	char *hash = malloc(HASH_LENGTH);

	strncpy(hash, other, HASH_LENGTH - 1);

	return hash;
}

char *getPassword(char *other) {
	char *password = malloc(PW_LENGTH);

	for(int i = 0; i < PW_LENGTH; i++)
		password[i] = other[i + HASH_LENGTH];
	
	return password;
}

unsigned int getPackage(char *other) {
	char tmp[3];
	int i = 0;

	while(other[i + HASH_LENGTH] != '\0') {
		tmp[i] = other[i + HASH_LENGTH];
		i++;
	}
	tmp[i] = '\0';

	char *ptr;
	unsigned int package = (unsigned int) strtol(tmp, &ptr, 10);
	printf("package = %u\n", package);

	return package;
}