/*
parallel(1) - executes independent Shell command lines in parallel, N at a time

usage - <sequence of independent Shell command lines> | parallel [-s SHELL] [N]

parallel(1) runs an arbitrarily large set of independent Shell command lines 
in parallel such that at exactly N of them will run simultaneously.

return value - parallel(1)'s exit code equals the number of children that exitted
with a non-zero status. 
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>

// Useful error messages
const char* ERROR_ARGC = "parallel: error: too many command line arguments\n";
const char* ERROR_MISSING_SHELL = "parallel: error: \"-s\" option is present but missing a value\n";
const char* ERROR_BAD_SHELL = "parallel: error: invalid value given to option \"-s\"\n";
const char* ERROR_UNKNOWN_N = "parallel: error: \"%s\" - unknown option or invalid value of N present\n";
const char* ERROR_INVALID_N = "parallel: error: N must be greater than 0\n";
const char* ERROR_NO_CORES = "parallel: error: no available processes (get_nprocs() <= 0)\n";

int main(int argc, char* argv[]){
    // check the number of command line arguments - there should be at most 4
    // (parallel, -s, SHELL, N)
    const int maxArgc = 4;
    if (argc > maxArgc){
        fprintf(stderr, ERROR_ARGC);
        return EXIT_FAILURE;
    }
    
    // parse command line arguments for -s and N, if present
    int i;
    int N = -1;
    char shell[256];
    strcpy(shell, getenv("SHELL")); // by default use the system's default shell
    char* endptr;
    for(i = 1; i < argc; ++i){
        if (strcmp(argv[i], "-s") == 0){
            if(++i >= argc){
                // if -s is present, but there's no more arguments,
                // then SHELL is not present, and the arguments are malformed.
                fprintf(stderr, ERROR_MISSING_SHELL);
                return EXIT_FAILURE;
            }else{
                // check SHELL to ensure that it's at least a string
                // - there's no good way to ensure it's a valid shell program
                strtol(argv[i], &endptr, 10); // floats? 
                if (*argv[i] != *endptr && *endptr == '\0'){
                    // successfully parsed a number - SHELL argument is invalid
                    fprintf(stderr, ERROR_BAD_SHELL);
                    return EXIT_FAILURE;
                }else{
                    // SHELL is good, save it for later
                    strcpy(shell, argv[i]);
                }
            }
        } 
        // check for N argument by attempting to parse a numeric value from argv[i]
        else if ((N=strtol(argv[i], &endptr, 10)) != -1 && 
            *argv[i] != *endptr && *endptr == '\0'){
            if (N <= 0){
                // can't have a negative number of parallel threads or cores
                fprintf(stderr, ERROR_INVALID_N, argv[i]);
                return EXIT_FAILURE;
            }
        }
        else{
            // argv[i] wasn't "-s" or a number, show an error that the option is unknown
            // and exit
            fprintf(stderr, ERROR_UNKNOWN_N, argv[i]);
            return EXIT_FAILURE;
        }
    }
    
    // ensure we have a value for N that's valid
    int C = get_nprocs(); // number of available cores 
    if (C <= 0){
        fprintf(stderr, ERROR_NO_CORES);
        return EXIT_FAILURE;
    }
    // force value of N to be C by default, and at most C
    if (N <= 0 || N > C){
        N = C;
    }
    //printf("N=%d\n",N);
    
    unsigned char countNonzeroChildren = 0; // return status counter
    int n = 0; // number of threads currently running 
    int* pids = (int*)malloc(sizeof(int)*N); // pids of child threads
    char buf[1024];
    int allDone=0; // flag to track if we're done executing commands
    while (allDone == 0){
        // start child processes until all done
        if (n < N){
            // if available, start a process
            if(fgets(buf, 1024, stdin) != NULL){
                // if we read a command, start it
                int i;
                // find first available child slot
                for(i = 0; i < N; ++i){
                    if (pids[i] == 0){
                        break;
                    }
                }
                // fork and exec the command
                int pid = pids[i] = fork();
                if(pid == -1){
                    return EXIT_FAILURE;
                }else if (pid==0){
                    execl(shell, shell, "-c", buf, 0);
                    perror("child exec failed");
                    return EXIT_FAILURE;
                }else{
                    // increment number of children
                    ++n;
                }
            }
            else if (n == 0){
                // didn't read a command, and no jobs to do, done
                allDone = 1;
            }
        }
        //printf("current pid: %d", getpid());
        // check the status of all child threads to see if we can start
        // a new command soon
        int i;
        //printf("waiting for children...\n");
        for(i = 0; i < N; ++i){
            if (pids[i] == 0){
                continue;
            }
            //printf("child num %d (pid %d)... ", i, pids[i]);
            int status = 0;
            // wait without blocking
            if( waitpid(pids[i], &status, WNOHANG) == pids[i]){
                // child thread has returned
                //printf("pid %d exited with status %d\n", pids[i], WEXITSTATUS(status));
                --n; // decrement number of children running 
                pids[i] = 0; // reset pids[i] value
                // determine the exit status of the child thread and increment
                // the counter if appropriate
                if(WIFEXITED(status) !=0 ){
                    if (WEXITSTATUS(status) != 0){
                        if (countNonzeroChildren < 255){
                            ++countNonzeroChildren;
                        }
                    }
                }else{
                    if (countNonzeroChildren < 255){
                        ++countNonzeroChildren;
                    }
                }
            }else{
                //printf("not done\n");
            }
        }
    }
    //printf("num non-zero children: %d\n",countNonzeroChildren);
    return countNonzeroChildren;
}