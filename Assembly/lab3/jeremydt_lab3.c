/*
    ICS 51. Lab #3

    IMPORTATNT NOTES:

    - This lab has five parts. The description of each part is given below.

    - Noramally, the compiler adds the prologue/epilogue code for the callee. However, when the 
    __declspec(naked) attribute is used, it is the programmer's responsibility to write the 
    prologue/epilogue code. In this lab you need to write prologue/epilogue code sections only 
    for the functions that have been declared with the naked attribute, namely:

       + findMinIndex
       + isPalindrome
       + performOperation
       + gcd

    - You cannot define any local variable in the C code. If you need additional variables in your
    assembly code, you have to allocate space for them in the prologue of the function. 

    - You are not allowed to change anything in this file except for adding your assembly code 
    between the lines marked "BEGIN YOUR CODE HERE" and "END YOUR CODE HERE".

    - Remember to fill in your name, student ID below.

    - ONLY submit the lab3.c file for grading.

*/

char *yourName = "Jeremy Taylor";
char *yourStudentID = "14336420";


/***********************************************************************************************

    PART 1: Change Letter Case

    You are given a string (null-terminated array of characters) that may contain numbers,
    upper andlower case letters, punctuations symbols, etc. Implement a function to convert all
    uppercase letters to lowercase and vice versa.
    For example "ThisIsTheFinalLabOf51" should be converted to "tHISiStHEfINALlABoF51".
    The string should be modifed in place. There is no explicit output for this function.

    Tips:

    - Note that each element of the arrays is a char (1 byte). You can NOT use a 32-bit register
    to read/write from/to a memory location. Instead you should use the 8-bit version of the x86
    general-purpose registers.

    - The link to the ASCII codes is here: http://www.asciitable.com/

************************************************************************************************/

void changeCase(char *string) {

    __asm{
        // BEGIN YOUR CODE HERE
		mov		esi, string
		mov		ecx, 0
	
	BeginWhile:
		cmp		byte ptr [esi + ecx], 00h
		je		EndWhile

		cmp		byte ptr [esi + ecx], 041h
		jl		Else
		cmp		byte ptr [esi + ecx], 05Ah
		jg		Else

		mov		al, byte ptr [esi + ecx]
		add		al, 020h
		mov		byte ptr [esi + ecx], al
		jmp		IncIndex
	Else:
		cmp		byte ptr [esi + ecx], 061h
		jl		IncIndex
		cmp		byte ptr [esi + ecx], 07Ah
		jg		IncIndex		
		
		mov		al, byte ptr [esi + ecx]
		sub		al, 020h
		mov		byte ptr [esi + ecx], al
	
	IncIndex:
		inc		ecx
		jmp		BeginWhile
	EndWhile:
        // END YOUR CODE HERE
    }
}

/***********************************************************************************************

    PART 2: Selection Sort

    Selection sort is an in-place comparison sort algorithm that works by dividing the input list 
    into two parts: the sublist of items already sorted, which is built up from left to right of 
    the list, and the sublist of items remaining to be sorted that occupy the rest of the list. 
    Initially, the sorted sublist is empty and the unsorted sublist is the entire input list. 
    The algorithm proceeds by finding the smallest element in the unsorted sublist, exchanging it 
    with the leftmost unsorted element (putting it in sorted order), and moving the sublist 
    boundaries one element to the right. To learn more, read: 
    https://en.wikipedia.org/wiki/Selection_sort#Example

    Our implementation here uses a function called "findMinIndex" to find the index of smallest
    element between ith index and jth index of the array. The function "selectionSort" uses
    this function to find the smallest number and exchanges it with the leftmost unsorted element
    in each iteration. You need to implement the behavior of both functions in x86 assembly.

************************************************************************************************/

__declspec(naked) 
int findMinIndex (int integer_array[], int i, int j)
{

// C code to be converted to x86 assembly
/*
    int iMin = i;
    // test against elements after i and before j to find the smallest 
    for ( i ; i < j; i++) {
        // if this element is less, then it is the new minimum   
        if (integer_array[i] < integer_array[iMin]) {
            // found new minimum; remember its index 
            iMin = i;
        }
    }

    return iMin;
*/

    __asm{
        mov eax, 0
        // BEGIN YOUR CODE HERE
		push	ebp
		mov		ebp, esp
			
		sub		esp, 4 // imin [ebp - 4]
			
		push	esi
		push	ebx
		push	ecx
		push	edx

		mov		esi, [ebp + 8] // array
		mov		ecx, [ebp + 12] // i
		mov		edx, [ebp + 16] // j

		mov		dword ptr [ebp - 4], ecx
	BeginWhile:
		cmp		ecx, edx
		jge		EndWhile

		mov		ebx, [ebp - 4]
		mov		ebx, [esi + ebx * 4] // arr[iMin]
		cmp		[esi + ecx * 4], ebx // arr[i] < arr[iMin]
		jge		IncIndex

		mov		dword ptr [ebp - 4], ecx
	IncIndex:
		inc		ecx
		jmp		BeginWhile

	EndWhile:
		mov		eax, [ebp - 4]
		
		pop		edx
		pop		ecx
		pop		ebx
		pop		esi

		mov		esp, ebp

		pop		ebp

        // END YOUR CODE HERE
        ret
    }

}

void selectionSort (int integer_array[], int array_size)
{

// C code to be converted to x86 assembly
/*  
    int j;
    int iMin;
    int temp;

    // advance the position through the entire array //
    // (could do j < n-1 because single element is also min element) //
    for (j = 0; j < array_size-1; j++) {

        // find the index of min element in the unsorted a[j .. n-1] //
        iMin = findMinIndex (integer_array, j, array_size);
 
        if(iMin != j) { // swap values
            temp = integer_array[iMin];
            integer_array[iMin] = integer_array [j];
            integer_array[j] = temp;    
        }
    }
*/

    __asm{
        // BEGIN YOUR CODE HERE
		mov		esi, integer_array
		mov		ecx, 0
		mov		edx, array_size
		dec		edx
	BeginWhile:
		cmp		ecx, edx 
		jge		EndWhile
		
		push	esi 
		push	ecx 
		push	edx 

		push	array_size
		push	ecx 
		push	esi
		
		call	findMinIndex

		add		esp, 12 
		
		pop		edx
		pop		ecx 
		pop		esi 

		cmp		eax, ecx 
		je		IncIndex 
		
		mov		ebx, [esi + eax * 4] // temp = arr[iMin]
		mov		edi, [esi + ecx * 4] // edi = arr[j]
		mov		[esi + eax * 4], edi // arr[iMin] = arr[j]
		mov		[esi + ecx * 4], ebx // arr[j] = temp

	IncIndex:
		inc		ecx 
		jmp		BeginWhile 
	EndWhile:
		
 
		// END YOUR CODE HERE
    }

}

/***********************************************************************************************

    PART 3: Palindrome

    Given a string, complete the implementation of the recursive function isPalindrome() to check 
    if the string is palindrome or not. A string is said to be palindrome if reverse of the 
    string is same as string. For example, “abba” is palindrome, but “abbc” is not palindrome.

************************************************************************************************/

__declspec(naked)
int isPalindrome(char *input_string, int left_index, int right_index)
{

// C code to be converted to x86 assembly
/*
    // Input Validation
    if (NULL == input_string || left_index < 0 || right_index < 0){
        return -1;
    }
    // Recursion termination condition
    if (left_index >= right_index)
        return 1;
    if (input_string[left_index] == input_string[right_index]){
        return isPalindrome(input_string, left_index + 1, right_index - 1);
    }
    return -1;
*/

    __asm{
		mov eax, 0
		// BEGIN YOUR CODE HERE
		
		push	ebp 
		mov		ebp, esp 
			
		push	esi 
		push	edi 
		push	ecx 
		push	edx
		push	ebx  
		
		xor		ebx, ebx
		mov		esi, [ebp + 8]
		mov		ecx, [ebp + 12]
		mov		edx, [ebp + 16]

		cmp		esi, 0
		je		IsNotPalindrome
		cmp		ecx, 0
		jl		IsNotPalindrome
		cmp		edx, 0
		jge		InputStringIsValid
		
		jmp		IsNotPalindrome 
	InputStringIsValid:
		cmp		ecx, edx 
		jl		Recursion
		
		jmp		RecursionEnd

	Recursion: 
		mov		bl, byte ptr [esi + ecx]
		cmp		bl, byte ptr [esi + edx]
		jne		IsNotPalindrome 

		push	ebx 
		push	ecx 
		push	edx 
		push	esi 

		dec		edx 
		push	edx 
		inc		ecx 
		push	ecx 
		push	esi 
		
		call	isPalindrome 

		add		esp, 12
		
		pop		esi 
		pop		edx 
		pop		ecx 
		pop		ebx 
		
		jmp		End 


	RecursionEnd:
		mov		eax, 1
		jmp		End
	IsNotPalindrome:
		mov		eax, -1
	End:
		pop		ebx 
		pop		edx 
		pop		ecx 
		pop		edi 
		pop		esi 

		pop		ebp

		// END YOUR CODE HERE
		ret
    }
}

/***********************************************************************************************

    PART 4: Vector Operation

    The vectorOperations function gets as parameters 4 arrays of the same length and
    the number of elements in those arrays. 

    For each pair of elements in first_array and second_array, it calls performOperation
    and stores the returned result in the same index of output_array.

    output_array[index] = performOperation(operation_array[index], first_array[index], second_array[index]).

    You also need to implement the body of performOperation function according to the provided
    C code.
  
************************************************************************************************/

__declspec(naked) 
int performOperation (int operation, int first, int second)
{

// C code to be converted to x86 assembly
/*
    if (operation==0) 
        return first - second;
    else if (operation==1)
        return first + second;
    else if (operation==2)
        return (first < second)? first : second;
    else if (operation==3)
        return (first > second)? first : second;
    else
        return first;
*/

    __asm {
        // BEGIN YOUR CODE HERE
		push	ebp
		mov		ebp, esp

		push	ebx 
		push	ecx 
		push	edx 

		mov		ebx, [ebp + 8] // operation 
		mov		ecx, [ebp + 12] // first 
		mov		edx, [ebp + 16] // second

		cmp		ebx, 0
		jne		ElseIf0 

		mov		eax, ecx 
		sub		eax, edx 
		jmp		End 
	ElseIf0:
		cmp		ebx, 1
		jne		ElseIf1

		mov		eax, ecx 
		add		eax, edx 
		jmp		End 
	ElseIf1:
		cmp		ebx, 2
		jne		ElseIf2

		cmp		ecx, edx
		jge		FirstIsGreater

		mov		eax, ecx 
		jmp		End 

		FirstIsGreater:
			mov		eax, edx
			jmp		End

	ElseIf2:
		cmp		ebx, 3
		jne		Else

		cmp		ecx, edx 
		jle		SecondIsGreater
		
		mov		eax, ecx
		jmp		End

		SecondIsGreater:
			mov		eax, edx 
			jmp		End 
	Else:
		mov		eax, ecx

	End:	
		pop		edx 
		pop		ecx 
		pop		ebx 
		pop		ebp 	
        // END YOUR CODE HERE
        ret
    }
  
}

void vectorOperations (int number_of_elements, int *first_array, int *second_array, 
                       int *operation_array, int *output_array)
{
// C code to be converted to x86 assembly
/*
    int i;
    for (i=0; i<number_of_elements; i++)
    {
        output_array[i] = performOperation(operation_array[i], first_array[i], second_array[i]);
    }
*/

    __asm {
        // BEGIN YOUR CODE HERE
		mov		esi, operation_array
		mov		edi, output_array
		mov		ebx, first_array
		mov		edx, second_array

		xor		ecx, ecx  
	BeginWhile:
		cmp		ecx, number_of_elements
		jge		EndWhile
		
		push	esi 
		push	edi 
		push	ebx 
		push	ecx 
		push	edx 

		push	[edx + ecx * 4]
		push	[ebx + ecx * 4]
		push	[esi + ecx * 4]

		call	performOperation

		add		esp, 12
		
		pop		edx
		pop		ecx
		pop		ebx 
		pop		edi 
		pop		esi 

		mov		[edi + ecx * 4], eax 
		
	
		inc		ecx 
		jmp		BeginWhile 
	EndWhile:

        // END YOUR CODE HERE
    }
}

/***********************************************************************************************

    PART 5: Dijkstra's Algorithm to calculate GCD.

    Implement a recursive function in assembly that calculates the greatest common divisor of 
    two unsigned integers.

    Given two unsigned integers n and m, we can define GCD(m , n) as:

           GCD(m , n) = n                       ,    if (m % n) == 0
           GCD(m , n) = GCD(n , m % n)          ,    if (m % n) >  0

************************************************************************************************/

__declspec(naked) 
unsigned int gcd(unsigned int m, unsigned int n) {

// C code to be converted to x86 assembly
/*
    if ((m % n) == 0)
        return n;
    else
        return gcd(n, m % n);
*/
    __asm{
        mov eax, 0
        // BEGIN YOUR CODE HERE
		push	ebp 
		mov		ebp, esp

		push	ebx 
		push	ecx 
		push	edx 
			
		mov		ebx, [ebp + 8] // m
		mov		ecx, [ebp + 12] // n
		
		xor		edx, edx 
		mov		eax, ebx 
		div		ecx 

		cmp		edx, 0
		jne		Else
		
		mov		eax, ecx 
		jmp		End
	Else:		
		push	ebx 
		push	ecx 

		xor		edx, edx 
		mov		eax, ebx 
		div		ecx 
		push	edx
		push	ecx 

		call	gcd

		add		esp, 8

		pop		ecx 
		pop		ebx 
		

	End:
		pop		edx 
		pop		ecx 
		pop		ebx 

		pop		ebp 

        // END YOUR CODE HERE
        ret
    }
}