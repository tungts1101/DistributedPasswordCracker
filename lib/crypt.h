/* 2018/11/20
 * version 1.0
 * */

#ifndef __CRYPT_H__
#define __CRYPT_H__

char *getNext(const char *from);
char *createFrom(unsigned int order);
char *solvePassword(char *other);
char *solve(char *from, char *to, char *hash);

#endif