/** 
 * every: prints M lines out of every N in a file or files
 * usage: $ every [-N,M] [list-of-files]
 * options/arguments:
 * -N,M: (optional) a pair of integers N > 0, M>=0 and M <= N
 * -list-of-files: (optional) a list of files to read. Must appear after -N,M, if present.
 * 
 * notes: If -N,M is not present, then the environment variable EVERY is checked. The program
 * expects EVERY to be in the format "N,M" with no leading dash. If EVERY is in a bad format
 * then it's ignored and the defaults are used.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Useful error messages
const char* ERROR_BAD_OPT_ORDER = "every: error: \"%s\" options must appear first in arguments\n";
const char* ERROR_BAD_OPT = "every: error: \"%s\" unknown option given\n";
const char* ERROR_NM = "every: error: N must be >0, M must be >=0, and M must be <= N\n";
const char* ERROR_BAD_FILE = "every: error: unable to open file \"%s\"\n";
const char* WARNING_BAD_ENV = "every: warning: environment variable EVERY formatted incorrectly, ignoring\n";

/* Parses a string in the format "N,M" into the parameters N,M 
*/
int parseNM(char* str, int* N, int* M);

/* Reads and prints M lines out of every N lines to stdout
*/
void every(FILE* file, int N,  int M);

int main(int argc, char* argv[]){
    // program will read every M lines out of N
    int N = 1; 
    int M = 1;

    int readEnv = 1; // if 1, check env variable EVERY for N,M

    int i = 1; // counter for parsing and validating arguments
    int j = i; // counter for iterating through filename arguments

    // parse and validate all given arguments
    for (; i < argc; ++i){
        if (argv[i][0] == '-'){
            if (i > 1){
                // the -N,M option can only be given first, 
                // before filenames. any other options aren't allowed
                fprintf(stderr, ERROR_BAD_OPT_ORDER, argv[i]);
                return EXIT_FAILURE;   
            }
            else{ // i == 0
                char* nmStr = argv[i]+1; // ignore leading dash (str is at least "-\0")
                int ret = parseNM(nmStr, &N, &M);
                if (ret == -1){
                    // option is given but with bad formatting of some kind - print an error and exit
                    fprintf(stderr, ERROR_BAD_OPT, argv[1]);
                    return EXIT_FAILURE;
                }else{
                    if (N <= 0 || M < 0 || M > N){
                        // -N,M is given but the values are illegal
                        fprintf(stderr, ERROR_NM, argv[1]);
                        return EXIT_FAILURE;
                    }
                    // else successfully parsed N,M
                    readEnv = 0;
                    j = 2; // start rest of arguments at 2
                }
                
            }
        }
    }

    // try to get N,M from the environment variable EVERY
    if (readEnv == 1){
        char* nm = getenv("EVERY");
        if (nm != NULL){
            if (parseNM(nm, &N, &M) == -1){
                // if EVERY is in a bad format, print a warning but continue
                // and use defaults
                fprintf(stderr, WARNING_BAD_ENV, nm);
            }
        }
    }

    int readStdin = 1; // if 1, read from stdin

    // open and read file arguments, if any
    for (; j < argc; ++j){
        readStdin = 0;
        FILE* file = fopen(argv[j], "r");
        if (file == NULL) {
            // if any file cannot be opened, then print an error but don't exit
            // (programs like 'cat' seem to do the same thing)
            fprintf(stderr, ERROR_BAD_FILE, argv[j]);
            continue;
        }
        // print lines and close file
        every(file, N, M);
        fclose(file);
    }

    // read from stdin if no file arguments
    if (readStdin == 1){
        every(stdin, N,M);
    }       

    return EXIT_SUCCESS;
}

int parseNM(char* str, int* N, int* M){
    if (strlen(str) < 1){
        return -1;
    }
    // attempt to parse values of N and M
    // note - not doing any extra validating of input string,
    // so input like -10,2,2,2 is allowed but only the first
    // two numbers get read
    int count = sscanf(str, "%u,%u",N,M);
    if (count < 1){
        // N,M is not present or given incorrectly
        return -1;
    }
    
    return 0;
}

void every(FILE* file, int N,  int M){
    char line[1024];
    
    int n = 0; // line counter
    while(fgets(line, 1024, file) != NULL){
        if (M == 0){
            // M == 0 is (apparently) allowed, but can't print
            // 0 lines of a file (still iterate over the file, though)
            continue; 
        }

        if (n % N < M || n % N == 0){
            // print every M lines in N, starting at 0th line
            // (n % N < M effectively counts how many
            // lines have been printed in a batch, up to M)
            printf(line);
        }

        ++n;
    }
}