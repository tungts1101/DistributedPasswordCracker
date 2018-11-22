/* 2018/11/20
 * version 1.0
 * */

#define _XOPEN_SOURCE
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../lib/crypt.h"
#include "../lib/config.h"
#include "../lib/other.h"

char *getNext(const char *from) {
	char *result = (char*)malloc(PW_LENGTH);
	strcpy(result, from);

	int i = PW_LENGTH;

	while(result[--i] == 'z');
	result[i]++;
	while(result[++i] == 'z')
		result[i] = 'a';

	return result;
}

char *solvePassword(char *other) {
	char *hash = getHash(other);
	char *from = getFrom(other);
	char *to = getTo(other);
	printf("from = %s, to = %s\n", from ,to);

	char  *password = solve(from, to, hash);
	printf("password = %s\n", password);

	return password;
}

// password combine only a-z
char *solve(char *from, char *to, char *hash) {
	char *iterator = (char *)malloc(PW_LENGTH);
	strcpy(iterator, from);

	while(strcmp(iterator, to) < 0) {
		if(strcmp(crypt(iterator, SALT), hash) == 0) {
			return iterator;
		}
		iterator = getNext(iterator);
	}
	
	return NULL;
}