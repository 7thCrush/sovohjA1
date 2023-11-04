#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include "morse.h"
#include "receive.h"

FILE *rfp;

int sigpipeR = 0;

void sig_hdr(int signo) {
    char ret = signo;
    int r = write(sigpipeR, &ret, 1);
    r = r; // shut up compiler
}

int receive(int pid, int ofd) {

    // receiver signal handler setup, can set up here because we only call this function once
    struct sigaction sig;
    int pipefd[2]; // read end pipefd[0], write end pipefd[1]
    assert(pipe(pipefd) == 0); // must be successful
    sigpipeR = pipefd[1]; // write end
    fcntl(sigpipeR, F_SETFL, O_NONBLOCK); // read and write won't block the process
    sigemptyset(&sig.sa_mask); // clear the signal mask, so that no signals are blocked while the signal handler is executing
    sig.sa_flags = SA_RESTART; // restart interrupted system calls
    sig.sa_handler = sig_hdr; // signal handler function
    assert((sigaction(SIGUSR1, &sig, NULL)) == 0); // all must be successful
    assert((sigaction(SIGUSR2, &sig, NULL)) == 0);
    assert((sigaction(SIGALRM, &sig, NULL)) == 0);
    assert((sigaction(SIGINT, &sig, NULL)) == 0);

    char code[7]; // morse code here, max length is 7 for them
    int j = 0;
    while (j < 7) { code[j++] = '\0'; } // strcmp will stop at the first '\0'
    int i = 0; // array index

    for ( ; ; ) { // keep receiving signals until mysignal == SIGINT
		char mysignal;
		int res = read(pipefd[0], &mysignal, 1);
		if (res < 0) perror("read failed");
		if (res == 1) {
        		if (mysignal == SIGUSR1) { // short signal ('.')
                    code[i++] = '.'; // set the character and then increment
                    int k = kill(pid, SIGUSR1); // note back that we received a signal
                    if (k == -1) {
                        fprintf(stderr, "Error sending an ACK.\n");
                        return -1; // if sending the signal failed
                    }
                } else if (mysignal == SIGUSR2) { // long signal ('-')
                    code[i++] = '-';
                    int k = kill(pid, SIGUSR1); // note back that we received a signal
                    if (k == -1) {
                        fprintf(stderr, "Error sending an ACK.\n");
                        return -1; // if sending the signal failed
                    }
                } else if (mysignal == SIGALRM) { // new morse code coming up
                    char c = decode(code); // get the appropriate character now that we know the full morse code has been received
                    fprintf(rfp, "Output: '%c'.\n", c);
                    int k = kill(pid, SIGUSR1); // note back that we received a signal
                    if (k == -1) {
                        fprintf(stderr, "Error sending an ACK.\n");
                        return -1; // if sending the signal failed
                    }
                    int l = 0;
                    while (l < 7) { code[l++] = '\0'; } // remake the empty buffer
                    int t = write(ofd, &c, 1); // write in the output
                    if (t < 0) {
                        fprintf(stderr, "Error writing in the output.\n");
                        return -1; // if writing failed
                    }
                    i = 0;
                } else if (mysignal == SIGINT) { // end of transmission
                    break;
                }
        }
	}

    return 0;
}
