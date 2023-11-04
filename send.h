#ifndef SEND
#define SEND

FILE *sfp;
extern int sigpipe;
extern int pipefd[2];
void sig_usr(int signo);
int sender(int pid, char c);

#endif
