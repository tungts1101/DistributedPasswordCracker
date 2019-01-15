/* 2018/11/13
 * version 1.0
 * */

#ifndef __COMMAND_H_
#define __COMMAND_H_

enum COMMAND {
    ACCEPT,
    JOIN,
    JOB,
    STOP,   // requester out of network
    QUIT,   // worker out of network
    PING,
    DONE_NOT_FOUND,
    DONE_FOUND,
    NOT_DONE,
    SHUTDOWN,
    HASH
};

static const char commandStr[11][15] = {
	"ACCEPT", "JOIN", "JOB", "STOP", "QUIT",
	"PING", "DONE_NOT_FOUND", "DONE_FOUND",
	"NOT_DONE", "SHUTDOWN", "HASH"
};

#endif
