/* 2018/12/10
 * version 1.0
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "../lib/structure.h"
#include "../lib/helper.h"

void testJobQueueAPI();
void testSolvePassword();

void main() {
	// testJobQueueAPI();
	testSolvePassword();
}

void testJobQueueAPI() {
	initJobQueue();
	struct Request request;
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
				request = createRequest(requestID, " ");
				splitJob(request);
				printJobQueue();
				break;
			case 2: ;
				printf("Remove all job with requestID = %d\n", requestID);
				removeJob(requestID);
				printJobQueue();
				break;
			case 3: ;
				printf("Get DONE_NOT_FOUND with requestID = %d, package = %d\n", requestID, package);
				deleteJob(requestID, package);
				printJobQueue();
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