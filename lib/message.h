/* 2018/11/13
 * version 1.0
 * */

#ifndef __MESSAGE_H_
#define __MESSAGE_H_

#include "command.h"
#include "config.h"

typedef struct Message_ {
    enum COMMAND command;
    unsigned int clientID;
    unsigned int requestID;
    char other[MSG_OTHER_LENGTH];
} Message;

// construct Message
Message request(enum COMMAND command, char *other);
Message response(enum COMMAND command, unsigned int clientID, unsigned requestID, char *other);

// handle Message.other
char *getHash(char *other);
char *getPassword(char *other);
unsigned int getPackage(char *other);

#endif
