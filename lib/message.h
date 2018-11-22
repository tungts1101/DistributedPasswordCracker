/* 2018/11/13
 * version 1.0
 * */

#ifndef __MESSAGE_H_
#define __MESSAGE_H_

#include "command.h"
#include "config.h"

struct Message {
    enum COMMAND command;
    unsigned int clientID;
    unsigned int requestID;
    char other[MSG_OTHER_LENGTH];
};

struct Message request(enum COMMAND command, char *other);
struct Message response(enum COMMAND command, unsigned int clientID, unsigned requestID, char *other);

#endif
