/* 2018/11/13
 * version 1.0
 * */

#ifndef __COMMAND_H_
#define __COMMAND_H_

enum COMMAND {
    ACCEPT,
    JOIN,
    JOB,
    ACK_JOB,
    PING,
    DONE_NOT_FOUND,
    DONE_FOUND,
    NOT_DONE,
    SHUTDOWN,
    HASH
};

static const char commandStr[10][15] = {
	"ACCEPT", "JOIN", "JOB", "ACK_JOB",
	"PING", "DONE_NOT_FOUND", "DONE_FOUND",
	"NOT_DONE", "SHUTDOWN", "HASH"
};

#endif
