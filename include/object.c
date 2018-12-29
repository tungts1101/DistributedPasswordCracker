/* 2018/11/13
 * version 1.0
 * */

#include <string.h>
#include <stdlib.h>
#include "../lib/object.h"

// CONNECTION =================================================================
void setConnection(Connection *conn, unsigned int clientID, int sockfd) {
	conn->clientID = clientID;
	conn->sockfd = sockfd;
}
// ============================================================================

// REQUEST ====================================================================
void setRequest(Request *req, unsigned int requestID, char *hash) {
	req->requestID = requestID;
	strcpy(req->hash, hash);	
}


// ============================================================================

// REQUESTER ==================================================================
void setRequester(Requester *req, unsigned int ID) {req->clientID = ID;};

// ============================================================================

// WORKER =====================================================================

// ============================================================================

// JOB ========================================================================
void resetJob(Job *j) {
	j->workerID = 0;
	j->requestID = 0;
	j->package = 0;
}
// ============================================================================