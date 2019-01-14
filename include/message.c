/* 2018/11/20
 * define a message structure
 * */

#include <string.h>
#include <stdlib.h>
#include "../lib/command.h"
#include "../lib/config.h"
#include "../lib/helper.h"
#include "../lib/message.h"

Message request(enum COMMAND command, char *other) {
	Message req;
	req.command = command;
	req.clientID = 0;
	req.requestID = 0;
	strcpy(req.other, other);

	return req;
}

Message response(enum COMMAND command, unsigned int clientID, unsigned requestID, char *other){
	Message res;
	res.command = command;
	res.clientID = clientID;
	res.requestID = requestID;
	strcpy(res.other, other);

	return res;
}

char *getHash(char *other) {
	char *hash = slice(other, 0, HASH_LENGTH - 1);
	return hash;
}

char *getPassword(char *other) {
	char *password = slice(other, HASH_LENGTH, HASH_LENGTH + PW_LENGTH);
	return password;
}

unsigned int getPackage(char *other) {
	char *tmp = slice(other, HASH_LENGTH, HASH_LENGTH + 3);
	return stringToUInt(tmp);
}
