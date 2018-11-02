#pragma once
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#define BUFSIZE (1024 * 256)

int sendOneByte(int socket, char* code); // 1 byte
int sendTwoBytes(int socket, int len); // 2 bytes
int sendFourBytes(int socket, int len); // 4 bytes
int sendNBytes(int socket, char *buf, int nbytes); // nbytes

int getOneByte(int socket, char* code);
int getTwoBytes(int socket, int* len);
int getFourBytes(int socket, int* len);
int getNBytes(int socket, char *buf, int nbytes);

//put
//get
//receiving pwd from server
//receiving dir from server

