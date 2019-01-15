/* 2018/11/14
 * version 1.0
 * */

#ifndef __ERROR_H__
#define __ERROR_H__

typedef enum {
   ERR_SOCK_FAILED,
   ERR_CONN_FAILED,
   ERR_SEND_FAILED,
   ERR_RECV_FAILED,
   ERR_BIND_FAILED,
   ERR_LIST_FAILED,
   ERR_ACCE_FAILED,
   ERR_NON_BLK_ASYNC,
   ERR_OWN_SOCKET,
   ERR_OPEN_LOGS,
   ERR_CLOSE_LOGS
} ErrorCode;

// kill process and notify about what error happened
void error(ErrorCode err);

#endif
