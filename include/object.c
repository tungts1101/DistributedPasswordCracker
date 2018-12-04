/* 2018/11/13
 * version 1.0
 * */

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
// ============================================================================

// WORKER =====================================================================
struct Worker createWorker(unsigned int clientID) {
	struct Worker w;

	w.clientID = clientID;

	return w;
}
// ============================================================================