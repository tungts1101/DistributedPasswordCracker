/* 2018/22/11
 * version 1.0
 * */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "../lib/structure.h"
#include "../lib/object.h"

struct Notice success() {
	struct Notice notice;

	notice.flag = SUCCESS;
	notice.reason = "OK";

	return notice;
}

struct Notice failure(char *reason) {
	struct Notice notice;
	notice.reason = malloc(strlen(reason));

	notice.flag = FAIL;
	strcpy(notice.reason, reason);

	return notice;
}

void init() {
	memset(&connectionList, 0, sizeof(connectionList));
	memset(&requesterList, 0, sizeof(requesterList));
	memset(&workerList, 0, sizeof(workerList));
	memset(&jobList, 0, sizeof(jobList));
}
// CONNECTION =================================================================
int getFirstConnection() {
	int i = -1;
	while(connectionList[++i].clientID != 0);
	return i;
}

unsigned int getNewClientID() {
	return getFirstConnection() + 1;
}

void addConnection(Connection conn) {
	setConnection(
		&connectionList[getFirstConnection()], conn.clientID, conn.sockfd);
}

void removeConnection(unsigned int clientID) {
	close(connectionList[clientID - 1].sockfd);
	memset(&connectionList[clientID - 1], 0, sizeof(Connection));
}

void deleteConnection() {
	for(int i = 0; i < MAX_PENDING; i++)
		removeConnection(i);
}

int getSocketDesc(unsigned int clientID) {
	return connectionList[clientID - 1].sockfd;
}

void printConnection() {
	printf("===== Connections: =====\n");

	for(int i = 0; i < MAX_PENDING; i++)
		if(connectionList[i].clientID != 0)
			printf("Client %u in socket %d\n", connectionList[i].clientID, connectionList[i].sockfd);
	
	printf("====================\n");
}
// ============================================================================

// REQUESTER ==================================================================


struct Notice addRequester(Requester requester) {
	int i = 0;
	
	while(requesterList[i++].clientID != 0);
		if(i == MAX_PENDING - 1)
			return failure("Requester list is already full");

	requesterList[--i] = requester;

	return success();
}

struct Notice addRequestToRequester(unsigned int clientID, Request request) {
	int i = 0;

	while(requesterList[i++].clientID != clientID);

	i--;
	int j = 0;
	while(requesterList[i].request[j++].requestID != 0);
	
	requesterList[i].request[--j] = request;

	return success();
}

unsigned int getRequesterFromRequest(unsigned int requestID) {
	int i = 0;
	int j;

	while(i < MAX_PENDING) {
		j = 0;
		while(j < MAX_REQUEST)
			if (requesterList[i].request[j++].requestID == requestID)
				return requesterList[i].clientID;
		
		i++;
	}
}

char *getHashFromRequest(unsigned int requestID) {
	int i = 0;
	int j;

	while(i < MAX_PENDING) {
		j = 0;
		while(j < MAX_REQUEST)
			if (requesterList[i].request[j++].requestID == requestID)
				return requesterList[i].request[--j].hash;
		
		i++;
	}
}

void printRequesterList() {
	printf("===== Requester List: =====\n");

	for(int i = 0; i < MAX_PENDING - 1; i++) {
		if(requesterList[i].clientID != 0) {
			printf("Requester %d:\n", requesterList[i].clientID);

			for(int j = 0; j < MAX_REQUEST; j++)
				if(requesterList[i].request[j].requestID != 0)
					printf("request ID = %d, hash = %s\n", requesterList[i].request[j].requestID,requesterList[i].request[j].hash);
		}
	}

	printf("====================\n");
}
// ============================================================================

// WORKER =====================================================================


struct Notice addWorkerList (Worker w) {
	int i = 0;	

	while(workerList[i++].clientID != 0)
		if(i == MAX_PENDING - 1)
			return failure("Worker list is already full");

	workerList[--i] = w;

	return success();
}

int getFirstWorker() {
	for(int i = 0; i < MAX_PENDING - 1; i++)
		if (workerList[i].clientID != 0 && workerList[i].jobNumber < MAX_JOB_PER_WORKER)
			return i;

	return -1;
}

void printWorkerList() {
	printf("===== Worker ID: =====\n");

	for(int i = 0; i < MAX_PENDING - 1; i++)
		if(workerList[i].clientID != 0)
			printf("%d has %d jobs", workerList[i].clientID, workerList[i].jobNumber);
	
	printf("\n====================\n");
}
// ============================================================================

// JOB ========================================================================
void setJob(Job *j, unsigned int workerID, unsigned int requestID, int package) {
	j->workerID = workerID;
	j->requestID = requestID;
	j->package = package;
}



int checkJobCondition(Job job) {
	return job.requestID != 0 && job.workerID == 0;
}

int getFirstJob() {
	for(int i = 0; i < MAX_JOB; i++)
		if(checkJobCondition(jobList[i]))
			return i;
	
	// if job queue is now full of worker
	return -1;
}

void splitJob(Request request) {
	int i = 0;
	
	for(int j = 1; j < 26; j++) {
		while(jobList[i++].requestID != 0);
		setJob(&jobList[--i], 0, request.requestID, j);
	}
}

void recoverJob(unsigned int clientID) {
	for(int i = 0; i < MAX_JOB; i++)
		if(jobList[i].workerID == clientID)
			jobList[i].workerID = 0;
}

struct Notice deleteJob(unsigned int requestID, unsigned int package) {
	for(int i = 0; i < MAX_JOB; i++)
		if(jobList[i].requestID == requestID && jobList[i].package == package) {
			jobList[i].workerID = 0;
			jobList[i].requestID = 0;
			jobList[i].package = 0;
		}
			
	return success();
}

struct Notice removeJob(unsigned int requestID) {
	for(int i = 0; i < MAX_JOB; i++) {
		if(jobList[i].requestID == requestID) {
			jobList[i].workerID = 0;
			jobList[i].requestID = 0;
			jobList[i].package = 0;	
		}
	}

	return success();
}

void printJobList() {
	printf("===== Job Queue =====\n");

	for(int i = 0; i < MAX_JOB; i++) {
		if(jobList[i].requestID != 0) {
			printf("WorkerID = %d, RequestID = %d, Packet = %d\n", jobList[i].workerID,jobList[i].requestID, jobList[i].package);
		}
	}	

	printf("=====================\n");
}
// ============================================================================