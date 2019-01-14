/* 2018/22/11
 * version 1.0
 * */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "../lib/structure.h"
#include "../lib/object.h"

// execute to reset all structure
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
	connectionList[getFirstConnection()] = conn;
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
int getFirstRequester() {
	int i = -1;
	while(requesterList[++i].clientID != 0);
	return i;
}
void addRequester(Requester requester) {
	requesterList[getFirstRequester()] = requester;
}
void removeRequester(unsigned int clientID) {
	int i = -1;
	while(requesterList[++i].clientID != clientID);
	memset(&requesterList[i], 0, sizeof(Requester));
}
void deleteRequester() {
	memset(&requesterList, 0, sizeof(requesterList));
}
void addRequestToRequester(unsigned int clientID, Request request) {
	int i = -1;
	while(requesterList[++i].clientID != clientID);

	int j = 0;
	while(requesterList[i].request[j++].requestID != 0);
	requesterList[i].request[--j] = request;
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
int getFirstWorker() {
	int i = -1;
	while(workerList[++i].clientID != 0);
	return i;
}
void addWorker(Worker w) {
	workerList[getFirstWorker()] = w;
}
void removeWorker(unsigned int clientID) {
	int i = -1;
	while(workerList[++i].clientID != clientID);
	memset(&workerList[i], 0, sizeof(Worker));
}
int getFirstEnableWorker() {
	for(int i = 0; i < MAX_PENDING - 1; i++)
		if (workerList[i].clientID != 0 && workerList[i].jobNumber < MAX_JOB_PER_WORKER)
			return i;

	return -1;
}
void removeJobFromWorker(unsigned int clientID) {
	for(int j = 0; j < MAX_PENDING - 1; j++)
		if(workerList[j].clientID == clientID) { // update job number
			workerList[j].jobNumber--;
			printf("Worker %d has job number %d\n", workerList[j].clientID, workerList[j].jobNumber);
			return;
		}
}
void printWorkerList() {
	printf("===== Worker ID: =====\n");

	for(int i = 0; i < MAX_PENDING - 1; i++)
		if(workerList[i].clientID != 0)
			printf("%d has %d jobs\n", workerList[i].clientID, workerList[i].jobNumber);
	
	printf("\n====================\n");
}
// ============================================================================

// JOB ========================================================================
int getFirstJob() {
	for(int i = 0; i < MAX_JOB; i++)
		if(jobList[i].requestID != 0 && jobList[i].workerID == 0)
			return i;
	
	// if job list is now full of worker
	return -1;
}
void assignJob(Worker *w, Job *j) {
	j->workerID = w->clientID;
	w->jobNumber++;
}
void setJob(Job *j, unsigned int workerID, unsigned int requestID, int package) {
	j->workerID = workerID;
	j->requestID = requestID;
	j->package = package;
}
void splitJob(Request request) {
	int i = -1;
	
	for(int j = 1; j < 26; j++) {
		while(jobList[++i].requestID != 0);
		setJob(&jobList[i], 0, request.requestID, j);
	}
}
void recoverJob(unsigned int clientID) {
	for(int i = 0; i < MAX_JOB; i++)
		if(jobList[i].workerID == clientID)
			jobList[i].workerID = 0;
}
void removeJob(unsigned int requestID, unsigned int package) {
	for(int i = 0; i < MAX_JOB; i++)
		if(jobList[i].requestID == requestID && jobList[i].package == package) {
			removeJobFromWorker(jobList[i].workerID);
			// update job list
			memset(&jobList[i], 0, sizeof(Job));
			return;
		}
}
void removeAllJobs(unsigned int requestID) {
	for(int i = 0; i < MAX_JOB; i++)
		if(jobList[i].requestID == requestID) {
			removeJobFromWorker(jobList[i].workerID);
			// update job list
			memset(&jobList[i], 0, sizeof(Job));
		}		
}
unsigned int *getWorkerFromRequest(unsigned int requestID) {
	unsigned int *worker = malloc(26 * sizeof(unsigned int));	// a magic number
	int j = 0;

	for(int i = 0; i < MAX_JOB; i++) {
		if(jobList[i].requestID == requestID) {
			int k = 0;

			for(; k < 26; k++)
				if(worker[k] == jobList[i].workerID)
					break;

			if(k == 26)		// add non-existed client to list
				worker[j++] = jobList[i].workerID;
		}
	}
	
	return worker;
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