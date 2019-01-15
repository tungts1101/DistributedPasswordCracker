/* 2018/11/21
 * version 1.0
 * */

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "config.h"

struct Connection_ {
	int clientID;
	int sockfd;
};
typedef struct Connection_ Connection;

struct Request_ {
	int requestID;
	char hash[HASH_LENGTH];
};
typedef struct Request_ Request;

struct Requester_ {
	int clientID;
	Request request[MAX_REQUEST];
};
typedef struct Requester_ Requester;

struct Worker_ {
	int clientID;
	int jobNumber;
};
typedef struct Worker_ Worker;

struct Job_ {
	int workerID;
	int requestID;
	int package;
};
typedef struct Job_ Job;

// CONNECTION =================================================================
	void setConnection(Connection *conn, int ID, int sockfd);
// ============================================================================

// REQUEST ====================================================================
	void setRequest(Request *req, int requestID, char *hash);
// ============================================================================

// REQUESTER ==================================================================
	void setRequester(Requester *req, int ID);
// ============================================================================

// WORKER =====================================================================

// ============================================================================

// JOB ========================================================================
	void setJob(Job *j, int workerID, int requestID, int package);
	void resetJob(Job *j);
// ============================================================================

#endif