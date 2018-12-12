/* 2018/22/11
 * version 1.0
 * */

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "config.h"
#include "object.h"
#include "helper.h"

enum FLAG{SUCCESS, FAIL};

Connection connectionList[MAX_PENDING];

Requester requesterList[MAX_PENDING - 1];

Worker workerList[MAX_PENDING - 1];

Job jobList[MAX_JOB];

struct Notice {
	enum FLAG flag;
	char *reason;
};

void init();

// CONNECTION =================================================================
	unsigned int getNewClientID();
	void addConnection(Connection conn);
	void removeConnection(unsigned int clientID);
	void deleteConnection();
	void printConnection();
	int getSocketDesc (unsigned int clientID);
// ============================================================================

// REQUESTER ==================================================================
	struct Notice addRequester(Requester requester);
	struct Notice addRequestToRequester(unsigned int clientID, Request request);
	unsigned int getRequesterFromRequest(unsigned int requestID);
	char *getHashFromRequest(unsigned int requestID);
	void printRequesterList();
// ============================================================================

// WORKER =====================================================================
	struct Notice addWorkerList (Worker w);

	/*
	 * use it when distributing job for worker
	 * */
	int getFirstWorker();
	
	void printWorkerList();
// ============================================================================

// JOB ========================================================================
	/*
	 * use it when distributing job for worker
	 * */
	int getFirstJob();

	/* 
	 * receive HASH command from requester
	 * split a request to 25 packages (from 'a' -> 'z')
	 * and add to jobList
	 * */
	void splitJob(Request request);

	/*
	 * when connection lost (check by PING)
	 * set all job.worker with that clientID to 0
	 * */
	void recoverJob(unsigned int clientID);

	/*
	 * receive DONE_NOT_FOUND command from worker
	 * (DONE_NOT_FOUND, workerID, requestID, package)
	 * */
	struct Notice deleteJob(unsigned int requestID, unsigned int package);

	/*
	 * receive DONE_FOUND command from worker
	 * (DONE_FOUND, workerID, requestID, package)
	 * remove all job with that requetsID from jobList
	 * */
	struct Notice removeJob(unsigned int requestID);

	void printJobList();
// ============================================================================

#endif