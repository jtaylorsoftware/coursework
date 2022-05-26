#include <stdio.h>
#include <conio.h>
#define  A     10

void main()
{
    int   max = 0, i = 0, sum = 0;
    int   int_array[ 11 ] = { 1,2,3,4,5,6,7,8,9,10,11 };

    /*
    for (i=0; i<A; i++)
    {
    sum +=int_array[i];
    }
    */

    // asm block to translate the above C for loop to x86 assembly
    __asm {
        MOV   EAX, 0
        MOV   ECX, 0

        START_ARRAY_FOR:
        cmp   ECX, A
            JG    END_ARRAY_FOR

            MOV   EDX, dword ptr int_array[ ECX * 4 ]
            ADD   EAX, EDX
            INC   ECX
            JMP   START_ARRAY_FOR

            END_ARRAY_FOR :
        MOV   sum, EAX
    }

    printf( "sum is %d\n", sum );
    _getch();
}
