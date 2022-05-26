/*
    ICS 51, Lab #2

    --------------------------------------------------------------------
 
    IMPORTATNT NOTES:
    
    - To test your code use the tests in lab2-testing.cpp
    
    - Write your assembly code only in the marked areas.
    
    - Do NOT change anything outside the marked areas.

    - Remember to fill in your name, student ID below.
 
    - This lab has to be done individually.

    - Two test images are provided to you (lena.png and authoritah.png)
    
    - Submit ONLY lab2.cpp file for grading.
*/


char *yourName = "Jeremy Taylor";
char *yourStudentID = "14336420";


// - This function takes in a square sized grayscale image and applies thresholding on each pixel.
//   i.e. it should change pixel values according to this formula:
//                     0xFF       if x >= threshold
//                     0x00       if x <  threshold
// - The width and height of the image are equal to dim. 
// - You are not allowed to define additional variables.
//
void imageThresholding(unsigned char* image, int dim, unsigned char threshold) {
    __asm {

            // YOUR CODE STARTS HERE
			xor		ebx, ebx						// clear ebx to ensure upper bits are off
			mov		bl, threshold					// store threshold in lower byte
			
			mov		eax, dim						// get the full length of the array in eax
			mul		dim								// 
			
			mov		edx, image						// store the base address of the pixels array in edx for access
			xor		ecx, ecx						// clear ecx to use as counter
	BeginWhile:
			cmp		ecx, eax						// loop while the index is not the last index
			je		EndWhile						// 
			
			mov		bh, byte ptr [edx + ecx]		// compare value in the image to the threshold
			cmp		bh, bl							//
			jb		BelowThreshold
			mov		byte ptr [edx + ecx], 0FFh		// if above or equal to threshold, set it to 0xFFh
			
			inc		ecx			
			jmp		BeginWhile
		BelowThreshold:
				mov		byte ptr [edx + ecx], 0		// if below threshold, set it to 0x00h

				inc		ecx
				jmp		BeginWhile
	EndWhile:
		
            // YOUR CODE ENDS HERE
    }
}



// - This function rotates a square sized color image 90 degress counterclockwise.
// - The width and height of the image are both equal to dim.
// - Four variables (i0, i90, i180, i270) are defined that you may use in your implementation for
//   temporary data storage. You are not allowed to define additional variables.
//
void imageRotation(unsigned int* image, int dim) {
  
    unsigned int i0, i90, i180, i270;
  
    __asm {
    
            // YOUR CODE STARTS HERE
			mov		esi, image

			xor		ecx, ecx

	BeginLoopRows:
			// loop over half the rows in image (red triangle)
			mov		ebx, dim
			shr		ebx, 1
			cmp		ecx, ebx
			je		EndLoopRows
			
			xor		ebx, ebx
			add		ebx, ecx
			
		BeginLoopColumns:
				// the first column index for red triangle is (row_base + row_number) and the last is (row_end - row_num - 1)
				mov		edi, dim
				sub		edi, ecx
				dec		edi
				cmp		ebx, edi
				je		EndLoopColumns

				// get index of p0 - spans the columns with indexes (row # -> row # - 1)
				mov		eax, ecx
				mul		dim
				add		eax, ebx
				
				mov		i0, eax

				// get index of p90 - for each row spans the column indexes (dim * dim - red_column - 1) + red_row
				mov		eax, dim
				sub		eax, ebx
				dec		eax
				mul		dim
				add		eax, ecx
				
				mov		i90, eax

				// get index of p180 - spans indexes dim * (dim - red_row) - red_col - 1
				mov		eax, dim
				sub		eax, ecx
				mul		dim
				sub		eax, ebx	
				dec		eax

				mov		i180, eax

				// get index of p270 - spans indexes (dim * red_column) + (dim - red_row - 1)
				mov		eax, ebx
				mul		dim
				add		eax, dim
				sub		eax, ecx
				dec		eax

				mov		i270, eax
			
				// save p90 value
				mov		edi, i90
				mov		edx, dword ptr [esi + edi * 4]
				
				// get p0 value, store in the image at i90
				mov		edi, i0
				mov		eax, [esi + edi * 4] // p0 value
				
				mov		edi, i90
				mov		dword ptr [esi + edi * 4], eax // store p0 value at p90

				// save p180 value and store in i90
				mov		edi, i180
				mov		eax, [esi + edi * 4] // p180 value
				mov		i90, eax			// store in i90

				// store p90 value (edx) in image at i180
				mov		edi, i180
				mov		dword ptr [esi + edi * 4], edx

				// save p270 value
				mov		edi, i270
				mov		edx, [esi + edi * 4]
				
				// store p180 value (i90) in eax and then store that value in memory at i270
				mov		eax, i90
				mov		edi, i270
				mov		dword ptr [esi + edi * 4], eax

				// move p270 value into memory at i0
				mov		edi, i0
				mov		dword ptr [esi + edi * 4], edx

				inc		ebx
				jmp		BeginLoopColumns
		EndLoopColumns:

			inc		ecx
			jmp		BeginLoopRows
	EndLoopRows:	
            // YOUR CODE ENDS HERE
    }
}