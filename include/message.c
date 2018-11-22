/* 2018/11/20
 * define a message structure
 * */

#include <string.h>
#include "../lib/message.h"

struct Message request(enum COMMAND command, char *other) {
	struct Message req;
	req.command = command;
	req.clientID = 0;
	req.requestID = 0;
	strcpy(req.other, other);

	return req;
}

struct Message response(enum COMMAND command, unsigned int clientID, unsigned requestID, char *other) {
	struct Message res;
	res.command = command;
	res.clientID = clientID;
	res.requestID = requestID;
	strcpy(res.other, other);

	return res;
}