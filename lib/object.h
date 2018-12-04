/* 2018/11/21
 * version 1.0
 * */

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "config.h"

struct Connection {
	unsigned int clientID;
	int sockfd;
};

struct Request {
	unsigned int requestID;
	char hash[HASH_LENGTH];
};

struct Requester {
	unsigned int clientID;
	struct Request *request;
};

enum STATUS{DONE, PENDING, NOT_FOUND};

struct Worker {
	unsigned int clientID;
};

struct Job {
	struct Worker worker;
	struct Request request;
	unsigned int packet;
	enum STATUS status;
};

// CONNECTION =================================================================
	struct Connection createConnection(unsigned int clientID, int sockfd);
// ============================================================================

// WORKER =====================================================================
	struct Worker createWorker(unsigned clientID);
// ============================================================================

#endif