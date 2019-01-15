/* 2018/11/14
 * version 1.0
 * */

#include <stdio.h>  // perror()
#include <stdlib.h> // exit()
#include "../lib/error.h"

#define NUM_OF_ERRORS 11

struct ErrorMessage {
    ErrorCode errorCode;
    char *message;
};

struct ErrorMessage errors[NUM_OF_ERRORS] = {
   {ERR_SOCK_FAILED, "socket() failed"},
   {ERR_CONN_FAILED, "connect() failed"},
   {ERR_SEND_FAILED, "send() sent a different number of bytes than expected."},
   {ERR_RECV_FAILED, "recv() failed or connection closed premeturely."},
   {ERR_BIND_FAILED, "bind() failed"},
   {ERR_LIST_FAILED, "listen() failed"},
   {ERR_ACCE_FAILED, "accept() failed"},
   {ERR_NON_BLK_ASYNC, "Error in setting socket to async, nonblock mode."},
   {ERR_OWN_SOCKET, "Error in setting own to socket."},
   {ERR_OPEN_LOGS, "Error when opening logs file."},
   {ERR_CLOSE_LOGS, "Error when closing logs file"}
};

void error(ErrorCode err) {
    for(int i = 0; i < NUM_OF_ERRORS; i++)
        if (errors[i].errorCode == err) {
            perror(errors[i].message);
            exit(1);
        }
}
