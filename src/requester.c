/* 2018/11/13
 * version 1.0
 * */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "../lib/message.h"
#include "../lib/connection.h"
#include "../lib/error.h"

#define MAX_LEN_BUF 30

int sockfd;
unsigned int clientID;

void signio_handler(int signo) {
	int n;
	struct Message res;

	if((n = recv(sockfd, (struct Message*)&res, sizeof res, 0)) > 0) {
        switch(res.command) {
			case ACCEPT:
				clientID = res.clientID;
				printf("Now my ID = %u\n", clientID);
				break;
			case DONE_NOT_FOUND:
                printf("Hash = %s\n", res.other);
				break;
            case DONE_FOUND: ;
                printf("Password = %s\n", res.other);
			default:
				break;
		}
    }
}

struct Request {
    unsigned int requestId;
    char password[PW_LENGTH];
};

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
        if (strcmp(outputStringArray[0],"HASH") == 0) {
            outputStringArray[1][strlen(outputStringArray[1])-1] = '\0';
            req = response(HASH, clientID, 0, outputStringArray[1]);
            send(sockfd, (struct Message*)&req, sizeof req, 0);
        } else {
            printf("Wrong connection type\n");
        }
        
        memset(temp_string,0,strlen(temp_string));

		// req = response(HASH, clientID, 0, "aac4aqibO4kDg");
        // send(sockfd, (struct Message*)&req, sizeof req, 0);
    }

    exit(0);
}