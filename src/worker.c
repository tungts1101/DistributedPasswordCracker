/* 2018/11/15
 * version 1.0
 * */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "../lib/message.h"
#include "../lib/connection.h"
#include "../lib/error.h"
#include "../lib/config.h"
#include "../lib/crypt.h"
#define MAX_LEN_BUF 30

int sockfd;
unsigned int clientID;

void signio_handler(int signo) {
	int n;
	struct Message res;
	struct Message req;
	char *hash;
	char *password;

	if((n = recv(sockfd, (struct Message *)&res, sizeof res, 0)) > 0) {
		switch(res.command) {
			case ACCEPT:
				clientID = res.clientID;
				printf("Now my ID = %u\n", clientID);
				break;
			case JOB: ;
				password = solvePassword(res.other);

				if(password == NULL)
					req = response(DONE_NOT_FOUND, clientID, res.requestID, "");
				else
					req = response(DONE_FOUND, clientID, res.requestID, password);

				send(sockfd, (struct Message *)&req, sizeof req, 0);
				break;
			default:
				break;
		}
	}
}

int main(int argc, char **argv)
{
    // TODO: advanced check for arguments list
    if (argc !=2) {
        exit(1);
    }

    sockfd = createTCPClientSocket(argv[1], SERV_PORT);

    if(fcntl(sockfd, F_SETFL, O_NONBLOCK | O_ASYNC) < 0)
        error(ERR_NON_BLK_ASYNC);

	signal(SIGIO, signio_handler);

	if(fcntl(sockfd, __F_SETOWN, getpid()) < 0)
        error(ERR_OWN_SOCKET);

    struct Message req;

    char* s = (char *)malloc(MAX_LEN_BUF * sizeof(char));
    char* temp_string = (char *)malloc(MAX_LEN_BUF * sizeof(char));
    char* splitString;
    int i = 0;
    char outputStringArray[MAX_LEN_BUF][MAX_LEN_BUF];

    while(fgets(s, MAX_LEN_BUF, stdin) != NULL) {
		i = 0;
        strcpy(temp_string,s);
        splitString = strtok(temp_string, "  \n");
        while (splitString != NULL) {
            strcpy(outputStringArray[i++], splitString);
            splitString = strtok(NULL, " ");
        }

		// Check loại connect
        if (strcmp(outputStringArray[0],"JOIN") == 0) {
            req = response(JOIN, clientID, 0, "");
            send(sockfd, (struct Message*)&req, sizeof req, 0);
        // } else if (strcmp(outputStringArray[0],"PING") == 0) {
    	// 	printf("PING not support\n");
        } else {
            printf("Wrong connection type\n");
        }
        
        memset(temp_string,0,strlen(temp_string));

    }

    exit(0);
}