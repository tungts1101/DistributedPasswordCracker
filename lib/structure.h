/* 2018/22/11
 * version 1.0
 * */

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "config.h"
#include "object.h"

enum FLAG{SUCCESS, FAIL};

struct Connection *connectionList;

struct Requester *requesterList;

struct Worker *workerList;

struct Job *jobQueue;

struct Notice {
	enum FLAG flag;
	char *reason;
};

// CONNECTION =================================================================
	void initConnection();
	unsigned int getNewClientID();
	struct Notice addConnection (struct Connection conn);
	struct Notice removeConnection (unsigned int clientID);
	struct Notice deleteConnection();
	void printConnection();
	int getSocketDesc (unsigned int clientID);
// ============================================================================

// REQUESTER ==================================================================
// ============================================================================

// WORKER =====================================================================
	void initWorkerList();
	struct Notice addWorkerList (struct Worker w);
	void printWorkerList();
// ============================================================================

// JOB ========================================================================
// ============================================================================

#endif