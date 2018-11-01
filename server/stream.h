#pragma once
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>

#define BUFSIZE (1024 * 256)

//int readn(int fd, char *buf, int bufsize);

//int writen(int fd, char *buf, int nbytes);

int sendCode(int socket, char* code); // 1 byte
int sendFNLen(int socket, int len); // 2 bytes
int sendFileSize(int socket, int len); // 4 bytes
int sendFN(int socket, char *buf, int nbytes); // nbytes

int getCode(int socket, char* code);
int getFNLen(int socket, int* len);
int getFileSize(int socket, int* len);
int getFN(int socket, char *buf, int nbytes);
