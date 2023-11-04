#define _POSIX_C_SOURCE 202009L
#define BLOCKSIZE 4096

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include "receive.h"
#include "send.h"
#include "morse.h"

int main(int argc, char **argv) {
    int ifd, ofd; // input and output file descriptors

    // command line parsing
    if (argc == 2) { // only input file, output stdout
        ofd = STDOUT_FILENO;
        if (strcmp(argv[1],"-") == 0) { // file1 is '-', standard input
            ifd = STDIN_FILENO;
        } else {
            ifd = open(argv[1],O_RDONLY); // file1 is a proper file, try to open it
            if (ifd < 0) {
                fprintf(stderr,"Opening input file (%s) failed\n",argv[1]);
                return -1;
            }
        }
    } else if (argc == 3) { // both input and output file given
        if (strcmp(argv[1],"-") == 0) { // file1 is '-', standard input
            ifd = STDIN_FILENO;
        } else { // file1 is a proper file, try to open it
            ifd = open(argv[1],O_RDONLY);
            if (ifd < 0) {
                fprintf(stderr,"Opening input file (%s) failed\n",argv[1]);
                return -1;
            }
        }
        if (strcmp(argv[2],"-") == 0) { // file2 is '-', output stdout
            ofd = STDOUT_FILENO;
        } else { // file2 is a proper file, try to open it (with or without creating it)
            ofd = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0644);
            if (ofd < 0) {
                fprintf(stderr,"Creating output file (%s) failed\n",argv[2]);
                return -1;
            }
        }
    } else { // otherwise instruct on how to use
        fprintf(stderr,"Usage: %s [input|-] [output|-]\n",argv[0]);
        return -1;
    }

    FILE *mfp = fopen("logmain.txt", "w"); // open main log file
    if (mfp == NULL) {
        fprintf(stderr, "Error opening the main log file!\n");
        return -1;
    }

    sfp = fopen("logsender.txt", "w"); // open server log file
    if (sfp == NULL) {
        fprintf(stderr, "Error opening the sender log file!\n");
        return -1;
    }

    rfp = fopen("logreceive.txt", "w"); // open receiver log file
    if (rfp == NULL) {
        fprintf(stderr, "Error opening the receiver log file!\n");
        return -1;
    }

    int parpid = getpid(); // parent's (A) id
    // process B creation
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Fork() failed\n");
        return -1;
    }
    if (pid == 0) { // process B (child)

        fprintf(mfp, "The child and parent are alive!\n");

        // signal handler setup for child (sender)
        // do this outside of the send function so that we don't do this every time 'sender' is called
        struct sigaction sig;
        assert(pipe(pipefd) == 0); // must be successful
        sigpipe = pipefd[1]; // write end
        fcntl(sigpipe, F_SETFL, O_NONBLOCK); // read and write won't block the process
        sigemptyset(&sig.sa_mask); // clear the signal mask, so that no signals are blocked while the signal handler is executing
        sig.sa_flags= SA_RESTART; // restart interrupted system calls
        sig.sa_handler = sig_usr; // signal handler function
        assert((sigaction(SIGUSR1, &sig, NULL)) == 0); // sender will only be receiving ACKs in this form

        char *buffer = malloc(BLOCKSIZE);
        if (buffer == NULL) { // if buffer allocation failed
            return -1;
        }
        
        while (1) {
            int bytes_read = read(ifd, buffer, BLOCKSIZE);
            if (bytes_read < 0) { // error
                fprintf(stderr, "Error reading from file descriptor.\n");
                return -1;
            }
            if (bytes_read == 0) { // end of file
                fprintf(mfp, "Reached end of the file.\n");
                break;
            }
            int i = 0;
            while (i < bytes_read) { // send data from the input
                char c = buffer[i++];
                //fprintf(fp, "Input: '%c'.\n", c);
                sender(parpid, c);
            }
        }

        kill(parpid, SIGINT); // end parent process (A)
        fprintf(mfp, "Notified parent process that we have sent everything.\n");
        free(buffer);

    } else { // process A (parent)

        receive(pid, ofd); // start receiving

    }

    close(ifd);
    close(ofd);
    fprintf(mfp, "Closed input and output.\n");
    fclose(mfp);
    fclose(sfp);
    fclose(rfp);
    
    return 0;
}
