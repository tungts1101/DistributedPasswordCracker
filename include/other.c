/* 2018/11/21
 * version 1.0
 * */

#include <stdlib.h>
#include <string.h>
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

char *getFrom(char *other) {
	char *from = malloc(PW_LENGTH);

	for(int i = 0; i < PW_LENGTH; i++)
		from[i] = other[i + HASH_LENGTH];
	
	return from;
}

char *getTo(char *other) {
	char *to = malloc(PW_LENGTH);

	for(int i = 0; i < PW_LENGTH; i++)
		to[i] = other[i + HASH_LENGTH + PW_LENGTH];
	
	return to;
}

char *setFrom(unsigned int order) {
	char *from = malloc(PW_LENGTH);

	memset(from, 'a', PW_LENGTH);
	from[0] = order + 96;

	return from;
}

char *setTo(char *from) {
      char *to = malloc(PW_LENGTH);
	  
      strcpy(to, from);
      to[0]++;

      return to;
}

char *getJob(char *hash, unsigned int order) {
	char *other = malloc(MSG_OTHER_LENGTH);

	char *from = setFrom(order);
	char *to = setTo(from);

	strcat(other, hash);
	strcat(other, " ");
	strcat(other, from);
	strcat(other, " ");
	strcat(other, to);

	return other;
}