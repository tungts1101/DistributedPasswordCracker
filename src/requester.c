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
#include "message.h"
#include "connection.h"
#include "error.h"

#define MAX_LEN_BUF 30
#define MAX_LEN_LINK 80

int sockfd = 0;
int clientID = 0;

int menu() {
    int kt;
    printf("\n\n\t _______________________________________________\n");
    printf("\t|         DISTRIBUTED PASSWORD CRACKER          |\n");
    printf("\t|_______________________________________________|\n");
    printf("\t|1.Choose from file                             |\n");
    printf("\t|2.Enter from keyboard                          |\n");
    printf("\t|3.See result                                   |\n");
    printf("\t|4.Exit                                         |\n");
    printf("\t|_______________________________________________|\n\n");
    do
        {
        printf("You choose? ");
        scanf("%d",&kt);
        if (kt != 1 && kt != 2 && kt != 3 && kt != 4)
            printf("Invalid option! Try again!\n");
        }
    while (kt != 1 && kt != 2 && kt != 3 && kt != 4);
    return kt;
}

struct Result {
    int requestID;
    char Hash[MAX_LEN_BUF];
    char result_msg[50];
    int count_nf;
    int status;
}; 

struct Result *resultQueue;

void initQueue() {
	resultQueue = (struct Result *) malloc(MAX_REQUEST * sizeof(struct Result));
}

void addToQueue(struct Result result) {
    int i = 0;
	
	while(resultQueue[i++].requestID != 0);

	resultQueue[--i] = result;
}

int validateHash(char* string) {
    return strlen(string) == 13;
}

void signio_handler(int signo) {
	int n;
    float progress;
	Message res;
    struct Result result;
    char output[10];

	if((n = recv(sockfd, (struct Message*)&res, sizeof res, 0)) > 0) {
        switch(res.command) {
			case ACCEPT:
				clientID = res.clientID;
                result.requestID = res.requestID;
                strcpy(result.result_msg,"0");
                strcpy(result.Hash,res.other);
                result.count_nf = 0;
                result.status = 0;
                addToQueue(result);
				break;
			case DONE_NOT_FOUND:
                for (int i=0; i < MAX_REQUEST; i++) {
                    if (resultQueue[i].requestID == res.requestID) {
                        if (resultQueue[i].status == 0) {
                            resultQueue[i].count_nf++;
                            progress = resultQueue[i].count_nf/25.0 * 100;
                            if (progress < 100)
                                snprintf(output, 50, "%f", progress);
                            else {
                                strcpy(output, "No password found");
                                resultQueue[i].status = 1;
                            }
                            strcpy(resultQueue[i].result_msg, output);
                        }
                        break;
                    }
                }
				break;
            case DONE_FOUND:
                strcpy(output,res.other);        
                for (int i=0; i < MAX_REQUEST; i++) {
                    if (resultQueue[i].requestID == res.requestID) {
                        strcpy(resultQueue[i].result_msg, output);
                        resultQueue[i].status = 1;
                        break;
                    }
                }      
                break;  
            case SHUTDOWN:
                exit(0);
                break;
			default:
				break;
		}
    }
}

void sendStopMsg() {
    Message msg = response(STOP, clientID, 0, "");
    send(sockfd, (struct Message *)&msg, sizeof msg, 0);
}

void sigintHandler(int sig_num) {
    sendStopMsg();
    exit(0);
}

struct Request {
    int requestId;
    char password[PW_LENGTH];
};

int main(int argc, char **argv)
{
    int port = SERV_PORT;
    // TODO: advanced check for arguments list
    if (argc != 2 && argc != 3) {
        exit(1);
    } else if (argc == 3) {
        port = atoi(argv[2]);
    }
    
    sockfd = createTCPClientSocket(argv[1], port);

    if(fcntl(sockfd, F_SETFL, O_NONBLOCK | O_ASYNC) < 0)
        error(ERR_NON_BLK_ASYNC);

	signal(SIGIO, signio_handler);
    signal(SIGINT, sigintHandler);

	if(fcntl(sockfd, __F_SETOWN, getpid()) < 0)
        error(ERR_OWN_SOCKET);

    Message req;

    int menu_check;
    char* hash_string = (char *)malloc(MAX_LEN_BUF * sizeof(char));
    char* file_link = (char *)malloc(MAX_LEN_BUF * sizeof(char));
    FILE *fin;
    menu_check = menu();

    initQueue();
    while (menu_check != 4)
        switch (menu_check) {
            case 1:
                printf("Enter File Destination: ");
                scanf("%s",file_link);

                if((fin = fopen(file_link, "r")) == NULL){
                    printf("Error: File not found\n");
                    continue;
                } else {
                    fgets(hash_string, MAX_LEN_BUF, fin);
                    hash_string[strlen(hash_string)-1] = '\0';
                    if (validateHash(hash_string)) {
                        // Send hash to server
                        printf("\nHash sent to server! Press 3 to see result!\n");
                        req = response(HASH, clientID, 0, hash_string);
                        send(sockfd, (struct Message*)&req, sizeof req, 0);
                        //
                    } else {
                        printf("\nHash length invalid! Hash length must be 13!\n");
                    }           
                }

                menu_check = menu();
                break;
            case 2:
                printf("Enter Hash String: ");
                scanf("%s",hash_string);

                if (validateHash(hash_string)) {
                    // Send hash to server
                    printf("\nHash sent to server! Press 3 to see result!\n");
                    req = response(HASH, clientID, 0, hash_string);
                    send(sockfd, (struct Message*)&req, sizeof req, 0);
                    //
                } else {
                    printf("\nHash length invalid! Hash length must be 13!\n");
                } 

                menu_check = menu();
                break;
            case 3:
                for (int i=0; i < MAX_REQUEST; i++) {
                    if (resultQueue[i].requestID != 0) {
                        printf("\n\n\t _______________________________________________\n");
                        printf("\t|               CRACKING RESULT                 |\n");
                        printf("\t|_______________________________________________|\n");
                        printf("\t|Hash: %20s                     |\n",resultQueue[i].Hash);
                        if (resultQueue[i].status == 1)
                            printf("\t|Result: %18s                     |\n",resultQueue[i].result_msg);
                        else 
                            printf("\t|Progress: %15s%%                     |\n",resultQueue[i].result_msg);
                        printf("\t|_______________________________________________|\n\n");
                    }
                }
                menu_check = menu();
                break;
        }
    if (menu_check == 4) {sendStopMsg(); printf("Thank you!\n");}

    exit(0);
}