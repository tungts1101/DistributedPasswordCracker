/* 2018/11/13
 * version 1.0
 * */

#include "../lib/object.h"

struct Connection createConnection(unsigned int clientID, int sockfd) {
	struct Connection conn;

	conn.clientID = clientID;
	conn.sockfd = sockfd;

	return conn;
}