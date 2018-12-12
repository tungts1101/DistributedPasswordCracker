/* 2018/11/15
 * version 1.0
 * */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define PW_LENGTH 5
#define SALT "aa"
#define HASH_LENGTH 14

#define MSG_OTHER_LENGTH 25

#define MAX_PENDING 10
#define SERV_PORT 3000

#define MAX_JOB_PER_WORKER 15

#define MAX_REQUEST 30

#define MAX_JOB ((MAX_PENDING - 1) * MAX_REQUEST * 25)

#endif