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

// CONNECTION =================================================================
void initConnection() {
	connectionList = (struct Connection *) malloc(MAX_PENDING * sizeof(struct Connection));

	for(int i = 0; i < MAX_PENDING; i++) {
		connectionList[i].clientID = 0;
		connectionList[i].sockfd = 0;
	}
}

int getConnectionLength() {
	int i = 0;
	while(connectionList[i++].clientID != 0);
	return i - 1;
}

unsigned int getNewClientID() {
	int i = 0;

	while(connectionList[i++].clientID != 0);

	return i;
}

struct Notice addConnection (struct Connection conn) {
	int i = 0;	

	while(connectionList[i++].clientID != 0)
		if(i == MAX_PENDING)
			return failure("Connections are out of bounds");

	connectionList[--i].clientID = conn.clientID;
	connectionList[i].sockfd = conn.sockfd;

	return success();
}

struct Notice removeConnection (unsigned int clientID) {
	int i = 0;

	while(connectionList[i++].clientID != clientID)
		if(i == MAX_PENDING)
			return failure("Connection cannot be found");
	
	close(connectionList[--i].sockfd);
	connectionList[i].clientID = 0;
	connectionList[i].sockfd = 0;

	return success();
}

struct Notice deleteConnection() {
	for(int i = 0; i < MAX_PENDING; i++)
		if(connectionList[i].sockfd != 0)
			close(connectionList[i].sockfd);

	free(connectionList);

	return success();
}

void printConnection() {
	printf("Connections:\n");

	for(int i = 0; i < MAX_PENDING; i++)
		if(connectionList[i].clientID != 0)
			printf("Client %u in socket %d\n", connectionList[i].clientID, connectionList[i].sockfd);
}

int getSocketDesc (unsigned int clientID) {
	int i = 0;

	while(connectionList[i++].clientID != clientID);

	return connectionList[--i].sockfd;
}
// ============================================================================

// REQUESTER ==================================================================

// ============================================================================

// WORKER =====================================================================
void initWorkerList() {
	// leave one slot for requester
	workerList = (struct Worker *) malloc((MAX_PENDING - 1) * sizeof(struct Worker));

	for(int i = 0; i < MAX_PENDING; i++) {
		workerList[i].clientID = 0;
	}
}

struct Notice addWorkerList (struct Worker w) {
	int i = 0;	

	while(workerList[i++].clientID != 0)
		if(i == MAX_PENDING - 1)
			return failure("Worker list is already full");

	workerList[--i].clientID = w.clientID;

	return success();
}

void printWorkerList() {
	printf("Worker ID: ");

	for(int i = 0; i < MAX_PENDING - 1; i++)
		if(workerList[i].clientID != 0)
			printf("%d ", workerList[i].clientID);
	
	printf("\n");
}
// ============================================================================

// JOB ========================================================================
// ============================================================================