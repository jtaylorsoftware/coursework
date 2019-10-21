/* Jeremy Taylor 14336420
 * ICS 53 Lab 4
 *
 * This program simulates a heap allocator that maintains an implicit free list.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEAP_SIZE 127
#define COMMAND_LEN 20

typedef unsigned char byte;

static int gBlockNumber = 0;
static byte gHeap[ HEAP_SIZE ];

int AllocateBlock( int size );
void FreeBlock( int blockNumToFree );
void WriteBlock( int blockNumToWrite, char character, int len );
void PrintBlocklist();
void PrintHeap( int blockNumToPrint, int len );
void PrintHeader( int blockNumToPrint );

int main()
{
    gHeap[ 0 ] = gBlockNumber++;
    gHeap[ 1 ] = ( 127 << 1 ) | 0;

    char command[ COMMAND_LEN ];
    int shouldQuit = 0;

    while ( shouldQuit != 1 )
    {
        printf( ">" );
        scanf( "%s", command );
        if ( strcmp( command, "allocate" ) == 0 )
        {
            int bytes = 0;
            scanf( "%i", &bytes );
            int blockNum = AllocateBlock( bytes + 2 );
            printf( "%i\n", blockNum );
        }
        else if ( strcmp( command, "free" ) == 0 )
        {
            int blockNum = 0;
            scanf( "%i", &blockNum );
            FreeBlock( blockNum );
        }
        else if ( strcmp( command, "blocklist" ) == 0 )
        {
            PrintBlocklist();
        }
        else if ( strcmp( command, "writeheap" ) == 0 )
        {
            int blockNum = 0;
            char character = 0;
            int len = 0;
            scanf( "%i %c %i", &blockNum, &character, &len );
            WriteBlock( blockNum, character, len );
        }
        else if ( strcmp( command, "printheap" ) == 0 )
        {
            int blockNum = 0;
            int bytes = 0;
            scanf( "%i %i", &blockNum, &bytes );
            PrintHeap( blockNum, bytes );
        }
        else if ( strcmp( command, "printheader" ) == 0 )
        {
            int blockNum = 0;
            scanf( "%i", &blockNum );
            PrintHeader( blockNum );
        }
        else if ( strcmp( command, "quit" ) == 0 )
        {
            shouldQuit = 1;
        }
    }

    return 0;
}

int AllocateBlock( int size )
{
    byte* block = gHeap;
    int blockNum = block[ 0 ];
    int blockSize = block[ 1 ] >> 1;
    int allocated = block[ 1 ] & 0x01;

    /* Find first free (not allocated) block */
    int currentByte = 0;
    while ( ( allocated == 1 || blockSize < size ) && currentByte < HEAP_SIZE )
    {
        block = block + blockSize;
        currentByte += blockSize;

        blockNum = block[ 0 ];
        blockSize = block[ 1 ] >> 1;
        allocated = block[ 1 ] & 0x01;
    }

    if ( currentByte >= HEAP_SIZE )
    {
        printf( "Invalid allocate\n" );
        return -1;
    }

    int actualBlockSize = size; /* the actual size of the block being allocated */
    /* Do split if needed */
    if ( blockSize > size )
    {
        int remainingSize = blockSize - size;
        if ( remainingSize <= 2 )
        {
            /* split would result in a block of size 2 bytes or smaller */
            actualBlockSize = blockSize;
        }
        else
        {
            /* split leaves enough room for header and some payload, so handle split */
            block[ 1 ] = ( size << 1 ) | 0;

            byte* splitBlock = block + size; /* get pointer to the smaller, split block */
            splitBlock[ 0 ] = 0;
            splitBlock[ 1 ] = ( remainingSize << 1 ) | 0;
        }
    }

    /* set block number and the size along with allocated flag */
    block[ 0 ] = gBlockNumber++;
    block[ 1 ] = ( actualBlockSize << 1 ) | 1;

    return block[ 0 ];
}

void FreeBlock( int blockNumToFree )
{
    byte* block = gHeap;
    int blockNum = block[ 0 ];
    int blockSize = block[ 1 ] >> 1;
    int allocated = block[ 1 ] & 0x01;

    /* find the block with the associated block number */
    int currentByte = 0;
    while ( blockNum != blockNumToFree && currentByte < HEAP_SIZE )
    {
        block = block + blockSize;
        currentByte += blockSize;

        blockNum = block[ 0 ];
        blockSize = block[ 1 ] >> 1;
        allocated = block[ 1 ] & 0x01;
    }

    if ( allocated != 1 || blockNum != blockNumToFree )
    {
        printf( "Invalid free\n" );
        return;
    }

    block[ 0 ] = 0; /* reset number to 0 */
    block[ 1 ] = block[ 1 ] & 0xFE; /* set allocated to 0 */

}

void WriteBlock( int blockNumToWrite, char character, int len )
{
    byte* block = gHeap;
    int blockNum = block[ 0 ];
    int blockSize = block[ 1 ] >> 1;
    int allocated = block[ 1 ] & 0x01;

    /* find the block with the associated block number */
    int currentByte = 0;
    while ( blockNum != blockNumToWrite && currentByte < HEAP_SIZE )
    {
        block = block + blockSize;
        currentByte += blockSize;

        blockNum = block[ 0 ];
        blockSize = block[ 1 ] >> 1;
        allocated = block[ 1 ] & 0x01;
    }

    if ( allocated == 0 || blockNum != blockNumToWrite )
    {
        /* don't write to a freed block or the incorrect block */
        return;
    }

    /* dont allow writes that would overflow */
    if ( len > blockSize - 2 )
    {
        printf( "Write too big\n" );
        return;
    }

    /* write into the block payload */
    byte* payload = block + 2;
    int i = 0;
    for ( ; i < len; ++i )
    {
        *payload++ = character;
    }
}

void PrintBlocklist()
{
    byte* block = gHeap;

    printf( "Size Allocated Start End\n" );

    /* Iterate over and print every block in the heap */
    int startAddress = 0;
    for ( ; startAddress < HEAP_SIZE; )
    {
        int blockSize = block[ 1 ] >> 1;
        int allocated = block[ 1 ] & 0x01;
        printf( "%-4i %-9s %-5i %-4i\n", blockSize, ( allocated == 1 ? "yes" : "no" ), startAddress, startAddress + blockSize - 1 );
        startAddress += blockSize;
        block = block + blockSize;
    }
}

void PrintHeap( int blockNumToPrint, int len )
{
    byte* block = gHeap;
    int blockNum = block[ 0 ];
    int blockSize = block[ 1 ] >> 1;
    int allocated = block[ 1 ] & 0x01;

    /* find the block with the associated block number */
    int currentByte = 0;
    while ( blockNum != blockNumToPrint && currentByte < HEAP_SIZE )
    {
        block = block + blockSize;
        currentByte += blockSize;

        blockNum = block[ 0 ];
        blockSize = block[ 1 ] >> 1;
        allocated = block[ 1 ] & 0x01;
    }

    if ( blockNum == blockNumToPrint )
    {
        int numToPrint = len > ( blockSize - 2 ) ? ( blockSize - 2 ) : len;
        int i = 0;
        byte* payload = block + 2;
        for ( ; i < numToPrint; ++i )
        {
            printf( "%c", *payload++ );
        }
        printf( "\n" );
    }
}

void PrintHeader( int blockNumToPrint )
{
    byte* block = gHeap;
    int blockNum = block[ 0 ];
    int blockSize = block[ 1 ] >> 1;

    /* find the block with the associated block number */
    int currentByte = 0;
    while ( blockNum != blockNumToPrint && currentByte < HEAP_SIZE )
    {
        block = block + blockSize;
        currentByte += blockSize;

        blockNum = block[ 0 ];
        blockSize = block[ 1 ] >> 1;
    }

    if ( blockNum == blockNumToPrint )
    {
        printf( "%02x%02x\n", block[ 0 ], block[ 1 ] );
    }
}
