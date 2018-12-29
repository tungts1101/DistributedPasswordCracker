/* 2018/11/21
 * version 1.0
 * */

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "config.h"

struct Connection_ {
	unsigned int clientID;
	int sockfd;
};
typedef struct Connection_ Connection;

struct Request_ {
	unsigned int requestID;
	char hash[HASH_LENGTH];
};
typedef struct Request_ Request;

struct Requester_ {
	unsigned int clientID;
	Request request[MAX_REQUEST];
};
typedef struct Requester_ Requester;

struct Worker_ {
	unsigned int clientID;
	unsigned int jobNumber;
};
typedef struct Worker_ Worker;

struct Job_ {
	unsigned int workerID;
	unsigned int requestID;
	unsigned int package;
};
typedef struct Job_ Job;

// CONNECTION =================================================================
	void setConnection(Connection *conn, unsigned int ID, int sockfd);
// ============================================================================

// REQUEST ====================================================================
	void setRequest(Request *req, unsigned int requestID, char *hash);
// ============================================================================

// REQUESTER ==================================================================
	void setRequester(Requester *req, unsigned int ID);
// ============================================================================

// WORKER =====================================================================

// ============================================================================

// JOB ========================================================================
	void setJob(Job *j, unsigned int workerID, unsigned int requestID, int package);
	void resetJob(Job *j);
// ============================================================================

#endif