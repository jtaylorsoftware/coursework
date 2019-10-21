/* Jeremy Taylor 14336420
 * ICS53 Lab3
 *
 * This program simulates a virtual memory system.
 */

#include <time.h>
#include <stdio.h>
#include <string.h>

#define MEM_ADDRESSES 8
#define MEM_PAGES MEM_ADDRESSES/2
#define DISK_ADDRESSES 16
#define DISK_PAGES DISK_ADDRESSES/2
#define PAGE_ENTRIES 8

typedef unsigned char byte;

typedef struct PageEntry
{
    byte valid;
    byte dirty;
    int pageNum; /* disk or physical page num */
} PageEntry;

int mainMemory[ MEM_ADDRESSES ] = { -1, -1, -1, -1, -1, -1, -1, -1 };
time_t mainTimeStamp[ MEM_PAGES ] = { 0 }; /*processor time stamp of when the physical page was last used, 0 meaning never used */
int disk[ DISK_ADDRESSES ] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

PageEntry pageTable[ PAGE_ENTRIES ] = {
    { 0, 0, 0}, { 0, 0, 1}, { 0, 0, 2}, { 0, 0, 3},
    { 0, 0, 4}, { 0, 0, 5}, { 0, 0, 6}, { 0, 0, 7} };

int DoRead( int address );
void DoWrite( int address, int value );
void ShowPPage( int pPageNum );
void ShowVPage( int vPageNum );
void PrintAddress( int address, int contents );
void ShowPageTable();

int FirstAvailablePage();

int main( int argc, char* argv[] )
{
    int shouldQuit = 0;

    char command[ 20 ];
    int arg0 = 0;
    int arg1 = 0;

    while ( shouldQuit == 0 )
    {
        printf( "$ " );
        scanf( "%s", command );
        if ( strcmp( command, "read" ) == 0 )
        {
            scanf( "%i", &arg0 );
            int val = DoRead( arg0 );
            printf( "%i\n", val );
        }
        else if ( strcmp( command, "write" ) == 0 )
        {
            scanf( "%i %i", &arg0, &arg1 );
            DoWrite( arg0, arg1 );
        }
        else if ( strcmp( command, "showmain" ) == 0 )
        {
            scanf( "%i", &arg0 );
            ShowPPage( arg0 );
        }
        else if ( strcmp( command, "showdisk" ) == 0 )
        {
            scanf( "%i", &arg0 );
            ShowVPage( arg0 );
        }
        else if ( strcmp( command, "showptable" ) == 0 )
        {
            ShowPageTable();
        }
        else if ( strcmp( command, "quit" ) == 0 )
        {
            shouldQuit = 1;
        }
    }
}

int DoRead( int address )
{
    int vPageNum = address >> 1;
    PageEntry* page = &pageTable[ vPageNum ];
    if ( page->valid == 1 )
    {
        int pPageNum = page->pageNum;
        int pAddress = pPageNum * 2 + ( address % 2 );
        mainTimeStamp[ pPageNum ] = time( NULL );
        return mainMemory[ pAddress ];
    }
    else
    {
        int pPageNum = FirstAvailablePage();
        mainTimeStamp[ pPageNum ] = time( NULL );
        page->valid = 1;
        page->pageNum = pPageNum;
        mainMemory[ pPageNum * 2 ] = disk[ vPageNum * 2 ];
        mainMemory[ pPageNum * 2 + 1 ] = disk[ vPageNum * 2 + 1 ];
        int pAddress = pPageNum * 2 + ( address % 2 );
        return mainMemory[ pAddress ];
    }
}

void DoWrite( int address, int value )
{
    int vPageNum = address >> 1;
    PageEntry* page = &pageTable[ vPageNum ];
    if ( page->valid == 1 )
    {
        int pPageNum = page->pageNum;
        page->dirty = 1;
        mainTimeStamp[ pPageNum ] = time( NULL );
        int pAddress = pPageNum * 2 + ( address % 2 );
        mainMemory[ pAddress ] = value;
    }
    else
    {
        int pPageNum = FirstAvailablePage();
        mainTimeStamp[ pPageNum ] = time( NULL );
        page->valid = 1;
        page->dirty = 1;
        page->pageNum = pPageNum;
        mainMemory[ pPageNum * 2 ] = disk[ vPageNum * 2 ];
        mainMemory[ pPageNum * 2 + 1 ] = disk[ vPageNum * 2 + 1 ];
        int pAddress = pPageNum * 2 + ( address % 2 );
        mainMemory[ pAddress ] = value;
    }
}

void ShowPPage( int pPageNum )
{
    printf( "%-10s %-10s\n", "Address", "Contents" );
    int pBaseAddr = pPageNum * 2;
    PrintAddress( pBaseAddr, mainMemory[ pBaseAddr ] );
    PrintAddress( pBaseAddr + 1, mainMemory[ pBaseAddr + 1 ] );
}

void ShowVPage( int vPageNum )
{
    printf( "%-10s %-10s\n", "Address", "Contents" );
    int vBaseAddr = vPageNum * 2;
    PrintAddress( vBaseAddr, disk[ vBaseAddr ] );
    PrintAddress( vBaseAddr + 1, disk[ vBaseAddr + 1 ] );
}

void PrintAddress( int address, int contents )
{
    printf( "%-10i %-10i\n", address, contents );
}

void ShowPageTable()
{
    printf( "%-10s %-10s %-10s %-10s\n", "VPageNum", "Valid", "Dirty", "PN" );
    int i;
    for ( i = 0; i < PAGE_ENTRIES; ++i )
    {
        PageEntry* e = &pageTable[ i ];
        printf( "%-10i %-10i %-10i %-10i\n", i, e->valid, e->dirty, e->pageNum );
    }
}

int FirstAvailablePage()
{
    int i;
    byte pagesInUse[ MEM_PAGES ] = { 0 }; /* 0 means not in use */
    for ( i = 0; i < PAGE_ENTRIES; ++i )
    {
        if ( pageTable[ i ].valid == 1 )
        {
            pagesInUse[ pageTable[ i ].pageNum ] = 1;
        }
    }

    int firstPage = -1;
    for ( i = 0; i < MEM_PAGES; ++i )
    {
        if ( pagesInUse[ i ] == 0 )
        {
            firstPage = i;
            break;
        }
    }

    if ( firstPage == -1 )
    {
        /* look for page with the lowest lastUsed that is in main memory */
        time_t oldestTime = mainTimeStamp[ 0 ];
        int minPPage = 0;  /* number of page least recently used in main memory */
        int minVPage = 0; /* corresponding virtual page number */
        for ( i = 1; i < MEM_PAGES; ++i )
        {
            if ( difftime(oldestTime, mainTimeStamp[i] ) > 0.0 ) /* if mainTimeStamp[i] is more recent than oldestTime, diffTime will be negative */
            {
                oldestTime = mainTimeStamp[ i ];
                minPPage = i;
            }
        }
        firstPage = minPPage;

        /* get the corresponding virtual page */
        for ( i = 0; i < PAGE_ENTRIES; ++i )
        {
            if ( pageTable[i].valid == 1 && pageTable[ i ].pageNum == minPPage )
            {
                minVPage = i;
                break;
            }
        }

        /* evict from main memory */
        pageTable[ minVPage ].valid = 0;
        if ( pageTable[ minVPage ].dirty == 1 )
        {
            disk[ minVPage * 2 ] = mainMemory[ minPPage * 2 ];
            disk[ minVPage * 2 + 1 ] = mainMemory[ minPPage * 2 + 1 ];
            pageTable[ minVPage ].dirty = 0;
        }
        pageTable[ minVPage ].pageNum = minVPage; /* since page table to disk is 1:1, set page table's number back to disk page since it's evicted */
    }

    return firstPage;
}
