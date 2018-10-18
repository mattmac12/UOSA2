#pragma once

#define BUFSIZE (1024 * 256)

int readn(int fd, char *buf, int bufsize);

int writen(int fd, char *buf, int nbytes);
