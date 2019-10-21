/* Jeremy Taylor 14336420
 * ICS53 Lab2
 *
 * This program uses multiple processes to search an array of integers for a specified value.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define NOT_FOUND -1
#define IS_FOUND(exitStatus) exitStatus != 255

#define MAX_ELEMENTS 10

int ReadNumberFile( FILE* numberFile, int numbersArray[] ); /* Reads integer values from numberFile */
int SearchArray( int value, int* arr, int start, int stop ); /* Searches arr for the given value within the given bounds */

int main( int argc, char* argv[] )
{
    if ( argc != 3 )
    {
        printf( "Invalid command line arguments\n" );
        return 1;
    }

    char* numberFileName = argv[ 1 ];
    int valueToFind = atoi( argv[ 2 ] );

    FILE* numberFile = fopen( numberFileName, "r" );
    if ( numberFile == NULL )
    {
        printf( "Unable to open file %s\n", numberFileName );
        return 1;
    }

    int numbersArray[ MAX_ELEMENTS ];
    int count = ReadNumberFile( numberFile, numbersArray );
    
    int index = SearchArray( valueToFind, numbersArray, 0, count - 1 );
    printf( "Search output: %i\n", index );
	
	fclose(numberFile);
}

int ReadNumberFile( FILE * numberFile, int numbersArray[] )
{
    int count = 0;
    while ( fscanf( numberFile, "%d", &numbersArray[ count ] ) != EOF )
    {
        ++count;
    }
    return count;
}

int SearchArray( int value, int* arr, int start, int stop )
{
    if ( start == stop )
    {
        printf( "pid %i, value: %i\n", getpid(), arr[ start ] ); /* if searching one element, do the comparison */
        fflush( stdin );
        return value == arr[ start ] ? start : NOT_FOUND;
    }

    int pidLeft = fork();
    if ( pidLeft == 0 )
    {
        /* only the left child process with pid == pidLeft will potentially recursively fork */
        int index = SearchArray( value, arr, start, ( start + stop ) / 2 ); /* child will recursively make more child processes */
        exit( index );
    }
    else if ( pidLeft < 0 )
    {
        return NOT_FOUND; /* failed to make a child process, can safely return NOT_FOUND */
    }
    else /* greater than 0 */
    {
        /* parent makes another fork for right child */
        int pidRight = fork();
        if ( pidRight == 0 )
        {
            /* right child does recursive search */
            int index = SearchArray( value, arr, ( start + stop ) / 2 + 1, stop );
            exit( index );
        }
        else if ( pidRight < 0 )
        {
            /* failed to make only the right child, must kill the left before returning */
            kill( pidLeft, SIGTERM );
            return NOT_FOUND; 
        }
        else /* greater than 0 */
        {
            /* parent of both pidLeft and pidRight, wait for processes to finish */
            int leftStatus = 0;
            int rightStatus = 0;
            waitpid( pidLeft, &leftStatus, 0 );
            waitpid( pidRight, &rightStatus, 0 );
            if ( WIFEXITED( leftStatus ) && WIFEXITED( rightStatus ) )
            {
                leftStatus = WEXITSTATUS( leftStatus );
                rightStatus = WEXITSTATUS( rightStatus );
            }
            return IS_FOUND( leftStatus ) ? leftStatus : IS_FOUND( rightStatus ) ? rightStatus : NOT_FOUND;
        }
    }
}
