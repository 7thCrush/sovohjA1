#ifndef RECEIVE_H
#define RECEIVE_H

FILE *rfp;
void sig_hdr(int signo);
int receive(int pid, int ofd);

#endif
