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

void testSolvePassword();
void testObject();
void testStructure();

void main() {
	// testSolvePassword();
	// testObject();
	testStructure();
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

// structure.h ================================================================
	void testConnectionList() {
		init();
		assert(1 == getNewClientID());

		Connection conn = {1, 4};
		addConnection(conn);
		assert(4 == getSocketDesc(1));
		assert(2 == getNewClientID());

		removeConnection(1);
		assert(1 == getNewClientID());
	}
	void testRequesterList() {
		init();
		Requester requester = {1, {1, "aajZvqJxbbrPI"}};
		addRequester(requester);

		Request request = {2, "aap9o6UZXOlDc"};
		addRequestToRequester(1, request);

		assert(1 == getRequesterFromRequest(2));
		assert(0 == strcmp("aap9o6UZXOlDc", getHashFromRequest(2)));

		removeRequester(1);
		assert(0 == requesterList[0].clientID);
		assert(0 == requesterList[0].request[0].requestID);
	}
	void testWorkerList() {
		init();
		Worker w_1 = {1, MAX_JOB};
		Worker w_2 = {2, 5};
		
		addWorker(w_1);
		addWorker(w_2);

		assert(1 == getFirstEnableWorker());
		removeWorker(1);
		Worker w_3 = {1, 7};
		addWorker(w_3);
		assert(1 == workerList[0].clientID);
	}
	void testJobList() {
		init();

		// requester sending 2 HASH
		Requester requester = {1, {0, ""}};
		addRequester(requester);

		Request req_1 = {1, "aajZvqJxbbrPI"};
		addRequestToRequester(1, req_1);
		splitJob(req_1);
		assert(0 == getFirstJob());

		Request req_2 = {2, "aap9o6UZXOlDc"};
		addRequestToRequester(1, req_2);
		splitJob(req_2);
		assert(0 == getFirstJob());

		// worker JOIN
		Worker w = {2, 0};
		addWorker(w);
		assert(0 == getFirstEnableWorker());
		assignJob(&workerList[getFirstEnableWorker()], &jobList[getFirstJob()]);
		assert(1 == getFirstJob());

		// worker solve password
		removeAllJobs(1);

		Request req_3 = {3, "aajZvqJxbbrPI"};
		addRequestToRequester(1, req_3);
		splitJob(req_3);
		assert(3 == jobList[0].requestID);
	}
	void testStructure() {
		testConnectionList();
		testRequesterList();
		testWorkerList();
		testJobList();
	}
// ============================================================================