#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include "morse.h"
#include "send.h"

FILE *sfp;

int sigpipe = 0;
int pipefd[2];

void sig_usr(int signo) {
    char ret = signo;
    int r = write(sigpipe, &ret, 1);
    r = r; // shut up compiler
}

int sender(int pid, char c) {


    char* code = encode(c); // get morse code
    int i = 0;
    fprintf(sfp, "Input: '%c'.\n", c);

    while (code[i] != '\0') { // while we have signals to send
        char ec = code[i]; // '.' or '-'
        if (ec == '.') { // SIGUSR1
            int k = kill(pid, SIGUSR1);
            if (k == -1) {
                fprintf(stderr, "Error sending a '.'.\n");
                return -1; // if sending the signal failed
            }
            i++;
        } else if ( ec == '-') { // SIGUSR2
            int k = kill(pid, SIGUSR2);
            if (k == -1) {
                fprintf(stderr, "Error sending a '-'.\n");
                return -1; // if sending the signal failed
            }
            i++;
        }
        for ( ; ; ) { // wait for ACK
            char mysignal;
            int res = read(pipefd[0], &mysignal, 1);
		    if (res < 0) perror("read failed");
		    if (res == 1 && mysignal == SIGUSR1) { // received ACK
                break;
            }
        }
    }
    int k = kill(pid, SIGALRM); // notify of new character
    if (k == -1) {
        fprintf(stderr, "Error sending a SIGALRM.\n");
        return -1; // if sending the signal failed
    }
    for ( ; ; ) { // wait for ACK
        char mysignal;
        int res = read(pipefd[0], &mysignal, 1);
		if (res < 0) perror("read failed");
		if (res == 1 && mysignal == SIGUSR1) { // received ACK
            break;
        }
    }

    return 0;
}
