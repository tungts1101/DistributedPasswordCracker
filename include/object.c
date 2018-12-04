/* 2018/11/13
 * version 1.0
 * */

#include <string.h>
#include <stdlib.h>
#include "../lib/object.h"

// CONNECTION =================================================================
struct Connection createConnection(unsigned int clientID, int sockfd) {
	struct Connection conn;

	conn.clientID = clientID;
	conn.sockfd = sockfd;

	return conn;
}
// ============================================================================

// REQUEST ====================================================================
struct Request createRequest(unsigned int requestID, char *hash) {
	struct Request r;

	r.requestID = requestID;
	strcpy(r.hash, hash);

	return r;
}
// ============================================================================

// REQUESTER ==================================================================
struct Requester createRequester(unsigned int clientID, struct Request request) {
	struct Requester r;

	r.clientID = clientID;

	// init request list
	r.request = (struct Request*) malloc(MAX_REQUEST * sizeof(struct Request));

	for (int i = 0; i < MAX_REQUEST; i++) {
		r.request[i].requestID = 0;
		strcpy(r.request[i].hash, "");
	}

	r.request[0].requestID = request.requestID;
	strcpy(r.request[0].hash, request.hash);

	return r;
}
// ============================================================================

// WORKER =====================================================================
struct Worker createWorker(unsigned int clientID) {
	struct Worker w;

	w.clientID = clientID;

	return w;
}
// ============================================================================