/* 2018/22/11
 * version 1.0
 * */

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "config.h"
#include "object.h"
#include "helper.h"

Connection connectionList[MAX_PENDING];

Requester requesterList[MAX_PENDING - 1];

Worker workerList[MAX_PENDING - 1];

Job jobList[MAX_JOB];

void init();

// CONNECTION =================================================================
	// handle when receiving HASH | JOING
	unsigned int getNewClientID();
	void addConnection(Connection conn);

	// handle when PING not found
	void removeConnection(unsigned int clientID);

	void deleteConnection();

	// debug only
	void printConnection();

	// handle when sending -> requester | worker
	int getSocketDesc (unsigned int clientID);
// ============================================================================

// REQUESTER ==================================================================
	// handle when receiving HASH
	void addRequester(Requester requester);

	// handle when PING not found
	void removeRequester(unsigned int clientID);
	void deleteRequester();

	// handle when receiving HASH
	void addRequestToRequester(unsigned int clientID, Request request);

	// handle when sending DONE_FOUND | DONE_NOT_FOUND -> requester
	unsigned int getRequesterFromRequest(unsigned int requestID);

	// handle when sending DONE_FOUND | DONE_NOT_FOUND -> requester
	char *getHashFromRequest(unsigned int requestID);

	// debug only
	void printRequesterList();
// ============================================================================

// WORKER =====================================================================
	// handle when receiving JOIN
	void addWorker(Worker w);

	// handle when PING not found
	void removeWorker(unsigned int clientID);
	void deleteWorker();
	
	// handle when sending JOB -> worker
	int getFirstEnableWorker();
	
	// debug only
	void printWorkerList();
// ============================================================================

// JOB ========================================================================
	// handle when sending JOB -> worker
	int getFirstJob();
	void assignJob(Worker *w, Job *j);

	// handle when receving HASH
	void splitJob(Request request);

	// handle when PING worker not found
	void recoverJob(unsigned int clientID);

	// handle when receiving DONE_NOT_FOUND
	void deleteJob(unsigned int requestID, unsigned int package);

	// handle when receving DONE_FOUND
	// handle when PING requester not found
	void removeJob(unsigned int requestID);

	// when receving DONE_FOUND
	// get a list of workers are solving that job
	unsigned int *getWorkerFromRequest(unsigned int requestID);

	// debug only
	void printJobList();
// ============================================================================

#endif