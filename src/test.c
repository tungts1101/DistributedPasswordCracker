/* 2018/12/10
 * version 1.0
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "../lib/structure.h"
#include "../lib/object.h"
#include "../lib/helper.h"

void testJobListAPI();

// helper.h ===================================================================
void testSolvePassword();
// ============================================================================

void testObject();

// structure ==================================================================
void testStructure();
void testConnectionList();
// ============================================================================

void main() {
	// testSolvePassword();
	// testObject();
	testStructure();
}

void testJobListAPI() {
	init();
	Request request;
	int command;
	unsigned int workerID;
	unsigned int requestID;
	unsigned int package;

	do {
		printf("get command : workerID : requestID : packet\n");
		scanf("%d %d %d %d", &command, &workerID, &requestID, &package);

		switch (command) {
			case 1: ;
				printf("Get new requestID = %d\n", requestID);
				setRequest(&request, requestID, " ");
				splitJob(request);
				printJobList();
				break;
			case 2: ;
				printf("Remove all job with requestID = %d\n", requestID);
				removeJob(requestID);
				printJobList();
				break;
			case 3: ;
				printf("Get DONE_NOT_FOUND with requestID = %d, package = %d\n", requestID, package);
				deleteJob(requestID, package);
				printJobList();
				break;
			default:
				break;
		}
	} while(command != 0);
}

void testSolvePassword() {
	char *other = malloc(MSG_OTHER_LENGTH);

	// pw = hdfgh -> hash = aajZvqJxbbrPI

	strcpy(other, "aajZvqJxbbrPI 1");
	assert(NULL == solvePassword(other));

	// solving password in package 8
	strcpy(other, "aajZvqJxbbrPI 8");
	assert(0 == strcmp("hdfgh", solvePassword(other)));


	strcpy(other, "aajZvqJxbbrPI 10");
	assert(NULL == solvePassword(other));
}

// object.h ===================================================================
void testConnectionObj() {
}

void testRequestObj() {
}

void testRequesterObj() {
}

void testWorkerObj() {
}

void testObject() {
	testConnectionObj();
	testRequestObj();
	testRequesterObj();
	testWorkerObj();
}
// ============================================================================

void testStructure() {
	init();
	testConnectionList();
}

void testConnectionList() {
	assert(1 == getNewClientID());

	Connection conn = {1, 4};
	addConnection(conn);
	assert(4 == getSocketDesc(1));
	assert(2 == getNewClientID());

	removeConnection(1);
	assert(1 == getNewClientID());
}