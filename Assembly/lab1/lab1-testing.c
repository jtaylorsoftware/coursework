#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <conio.h>

extern void minMax(int a, int b, int c, int *result);
extern int swapBits(int x);

extern char *yourName;
extern char *yourStudentID;
static void testcode();

int main(void)
{
	testcode();
	_getch();
	return 0;
}

static void testcode (void)
{
	int result1;
	int correct_answer1;
	int *result2;
	int correct_answer2;

	int c = 56;

	printf ("ICS 51, Lab 1 \nName:      %s\nStudentID: %s\n\n", 
	  yourName, yourStudentID);

  /* Once you are done implementing the functions, activate one or more of
     the following test sequences.
     Procedure to activate a single test sequence:
         replace 
              #define TESTPART1_1 0
         with
              #define TESTPART1_1 1
  */

	#define TESTPART1_1 1
	#define TESTPART1_2 1
	#define TESTPART1_3 1

	#define TESTPART2_1 1
	#define TESTPART2_2 1
	#define TESTPART2_3 1
	#define TESTPART2_4 1
	#define TESTPART2_5 1

	printf( "\nTesting part I \n" );
	#if TESTPART1_1
		result1 = swapBits(0xAAAAAAAA);
		correct_answer1 = 0x55555555;
		printf( "Input = 0x%08X, Output = 0x%08X - %s\n", 0xAAAAAAAA, result1, result1==correct_answer1 ? "CORRECT": "INCORRECT" );
	#endif

	#if TESTPART1_2
		result1 = swapBits(43);
		correct_answer1 = 23;
		printf( "Input = 0x%08X, Output = 0x%08X - %s\n", 43, result1, result1==correct_answer1 ? "CORRECT": "INCORRECT" );
	#endif

	#if TESTPART1_3
		result1 = swapBits(0x12345678);
		correct_answer1 = 0x2138A9B4;
		printf( "Input = 0x%08X, Output = 0x%08X - %s\n", 0x12345678, result1, result1==correct_answer1 ? "CORRECT": "INCORRECT" );
    #endif


	printf( "\nTesting part II \n" );
	#define MULTIPLIER 2017
	#if TESTPART2_1
		result2 = (int*) malloc(sizeof(int));
		minMax(1,2,3,result2);
		correct_answer2 = (1 + 3) * MULTIPLIER;
		printf( "Input = %d, %d, %d, Output = %d - %s\n", 1, 2, 3, *result2, *result2==correct_answer2 ? "CORRECT": "INCORRECT" );
		free(result2);
	#endif

	#if TESTPART2_2
		result2 = (int*) malloc(sizeof(int));
		minMax(4,2,7,result2);
		correct_answer2 = (2 + 7) * MULTIPLIER;
		printf( "Input = %d, %d, %d, Output = %d - %s\n", 4, 2, 7, *result2, *result2==correct_answer2 ? "CORRECT": "INCORRECT" );
		free(result2);
	#endif

	#if TESTPART2_3
		result2 = (int*) malloc(sizeof(int));
		minMax(3,2,1,result2);
		correct_answer2 = (1 + 3) * MULTIPLIER;
		printf( "Input = %d, %d, %d, Output = %d - %s\n", 3, 2, 1, *result2, *result2==correct_answer2 ? "CORRECT": "INCORRECT" );
		free(result2);
	#endif

	#if TESTPART2_4
		result2 = (int*) malloc(sizeof(int));
		minMax(-50,2,-100,result2);
		correct_answer2 = (-100 + 2) * MULTIPLIER;
		printf( "Input = %d, %d, %d, Output = %d - %s\n", -50, 2, -100, *result2, *result2==correct_answer2 ? "CORRECT": "INCORRECT" );
		free(result2);
	#endif
	
	#if TESTPART2_5
		result2 = (int*) malloc(sizeof(int));
		minMax(0,0,0,result2);
		correct_answer2 = (0 + 0) * MULTIPLIER;
		printf( "Input = %d, %d, %d, Output = %d - %s\n", 0, 0, 0, *result2, *result2==correct_answer2 ? "CORRECT": "INCORRECT" );
		free(result2);
	#endif

	printf( "\n\n" );
}