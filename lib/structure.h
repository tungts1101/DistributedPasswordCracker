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
	int getNewClientID();
	void addConnection(Connection conn);

	// handle when PING not found
	void removeConnection(int clientID);

	void removeAllConnections();

	// debug only
	void printConnection();

	// handle when sending -> requester | worker
	int getSocketDesc (int clientID);

	// handle when broadcasting DONE_FOUND
	int getClientIDFromSocket(int sockfd);
// ============================================================================

// REQUESTER ==================================================================
	// handle when receiving HASH
	void addRequester(Requester requester);

	// handle when PING not found
	int *getRequestFromRequester(int clientID);
	void removeRequester(int clientID);
	void removeAllRequesters();

	// handle when receiving HASH
	void addRequestToRequester(int clientID, Request request);

	// handle when sending DONE_FOUND | DONE_NOT_FOUND -> requester
	int getRequesterFromRequest(int requestID);

	// handle when sending DONE_FOUND | DONE_NOT_FOUND -> requester
	char *getHashFromRequest(int requestID);

	int checkRequesterExist(int clientID);

	// debug only
	void printRequesterList();
// ============================================================================

// WORKER =====================================================================
	// handle when receiving JOIN
	void addWorker(Worker w);

	// handle when PING not found
	void removeWorker(int clientID);
	void removeAllWorkers();
	
	// handle when sending JOB -> worker
	int getFirstEnableWorker();
	
	// handle when solving one job
	void removeJobFromWorker(int clientID);

	int checkWorkerExist(int clientID);

	// debug only
	void printWorkerList();
// ============================================================================

// JOB ========================================================================
	// handle when sending JOB -> worker
	int getFirstJob();
	void assignJob(Worker *w, Job *j);

	// handle when receving HASH
	void splitJob(Request request);

	// handle when PING worker not succeed
	void recoverJob(int clientID);

	// handle when receiving DONE_NOT_FOUND
	void removeJob(int requestID, int package);

	// handle when receving DONE_FOUND
	// handle when PING requester not found
	void removeAllJobs(int requestID);

	// when receving DONE_FOUND
	// get a list of workers are solving that job
	int *getWorkerFromRequest(int requestID);

	// debug only
	void printJobList();
// ============================================================================

#endif