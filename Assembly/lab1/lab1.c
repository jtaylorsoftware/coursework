#include <stdio.h>
#include <ctype.h>

/*
	ICS 51, Lab #1
	
	--------------------------------------------------------------------

	PART I: 

	Given a 32-bit integer, swap all odd bits with even bits. 
	For example, if the given number is 23 (00010111), it should be 
	converted to 43 (00101011). Every even position bit is swapped 
	with adjacent bit on right side, and every odd position bit is 
	swapped with adjacent on left side.

	Implementation details:
	The input integer is stored in registers ebx. You need to store the 
	answer into register eax.

	Hint: 
	You only need a half-dozen instructions to do this. 
	Think about "clearing" odd (even) bits and using shift (shl, shr) 
	instructions.
	
	If we have an 8-bit number 00010111, the expected output is 00101011.
	Clearing (zeroing) odd bits (7, 5, 3, 1) of 00010111 would make it 
	00010101. Now if we shift it to the left by one bit, it becomes 
	00101010. The odd bits of 00101010 are equal to the odd bits of the 
	expected output. They are "0 1 1 1".

	Using the same technique, generate the even bits of the expected 
	output and then figure out a way for combining generated odd and 
	even bits. 

	--------------------------------------------------------------------

	PART II: 
	
	You are given three 32-bit signed integers. You need to add the 
	smallest number to the largest number and multiply that sum by 2017. 
	You cannot use mul/imul instructions.

	Implementation details:
	The three integers are stored in registers eax, ebx, and ecx. You 
	need to store the answer into register eax.

	Hint:
	One way of finding minimum and maximum number is to sort the numbers.

	--------------------------------------------------------------------

	IMPORTATNT NOTES:
	
	- To test your code see the "testcode" function in lab1-testing.c
	
	- Write your assembly code only in the marked blocks.
	
	- Do NOT change anything outside the marked blocks.
	
	- For part II, you are NOT allowed to use any version of the 
      MULTIPLY instruction!
	
	- Remember to fill in your name, student ID below.

	- This lab has to be done individually.
	
	- Submit ONLY lab1.c file for grading.

*/

char *yourName = "Jeremy Taylor";
char *yourStudentID = "14336420";

/* Implement the body of this function for part I*/
int swapBits(int x)
{
	int result;

	__asm 
	{
		mov ebx, x

// YOUR CODE STARTS HERE
        mov eax, ebx
        and eax, 0AAAAAAAAh 
        shr eax, 1
        shl ebx, 1
        and ebx, 0AAAAAAAAh
        or  eax, ebx

	
// YOUR CODE ENDS HERE
 
		mov result, eax
	}
	return result;

}

/* Implement the body of this function for part II*/
void minMax(int a, int b, int c, int *result)
{
	__asm 
	{
		mov esi, result

		mov eax, a
		mov ebx, b
		mov ecx, c

// YOUR CODE STARTS HERE

        // First test if a > b
        cmp eax, ebx
        jl  EbxGreater
        // If a > b, move a into edx (used as 'greatest' value register)
        // and move b into edi (used as 'least' value register)
        mov edx, eax
        mov edi, ebx
        jmp Cmp1
    EbxGreater:
        // Alternatively, move b into edx and a into edi
        mov edx, ebx
        mov edi, eax

    Cmp1:
        // Compare c to result of first comparison.
        cmp edx, ecx
        jl  EcxGreater

        // If edx > c then do no moves but compare c to least value.
        cmp ecx, edi
        jl  EdiGreaterEcx
        // If ecx > edi then c is the middle value.
        jmp AddGreatestToLeast
    EdiGreaterEcx:
        // c is less than the current 'least' value so update least value to c.
        mov edi, ecx
        jmp AddGreatestToLeast
                
    EcxGreater:
        // c was greater than the current 'greatest'.
        // Now: Greatest: C, Middle: Edx (whatever value before the move), Least: Edi
        mov edx, ecx
        jmp AddGreatestToLeast
    
    AddGreatestToLeast:
        // Add greatest to least
        add edx, edi
        
		// Keep a copy of original number 
        mov edi, edx
		

		// Do multiplication by decomposing 2017 into powers of two:
		// -do a shift of the original value and add it to eax as the running "product"
		// -can multiply by 2016 with shifts, then add original value back in to get 2017
        shl edx, 10
		mov eax, edx
		
		mov edx, edi
		shl edx, 9
		add eax, edx

		mov edx, edi
		shl edx, 8
		add eax, edx
		
		mov edx, edi
		shl edx, 7
		add eax, edx

		mov edx, edi
		shl edx, 6
		add eax, edx

		mov edx, edi
		shl edx, 5
		add eax, edx
		
		// Add in the original value to get value * 2017
		add eax, edi


// YOUR CODE ENDS HERE

		mov [esi][0], eax
	}

	return;
}