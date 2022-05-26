#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define ANSI_COLOR_RED     /*"\x1b[31m"*/
#define ANSI_COLOR_GREEN   /*"\x1b[32m"*/
#define ANSI_COLOR_YELLOW  /*"\x1b[33m"*/
#define ANSI_COLOR_RESET   /*"\x1b[0m"*/

// Once you are done implementing the required functions, enable the testcases        
#define TEST_PART1 1
#define TEST_PART2 1
#define TEST_PART3 1
#define TEST_PART4 1 
#define TEST_PART5 1 

extern char *yourName;
extern char *yourStudentID;

// These functions should be implemented in lab3.c
extern void changeCase(char *string);
extern void selectionSort( int integer_array[], int array_size );
extern int isPalindrome(char *input_string, int left_index, int right_index);
extern void vectorOperations (int num_of_testcases, int *first_array, int *second_array, 
	                          int *operation_array, int *output_array);
extern unsigned gcd(unsigned n, unsigned m);

void test_changeCase();
void test_selectionSort();
void test_isPalindrome();
void test_vectorOperations();
void test_gcd ();

unsigned int total_num_of_tests = 0;
unsigned int total_num_of_tests_passed = 0;

/****************************************************************************************/

int main(void)
{
    printf ("ICS 51, Lab 3 \nName: %s\nStudent ID: %s\n\n",yourName, yourStudentID);

    #if TEST_PART1
        printf("====================== PART1 ======================\n");
        test_changeCase();
        printf("\n");
    #endif

	#if TEST_PART2
		printf("====================== PART2 ======================\n");
		test_selectionSort();
		printf("\n");
	#endif

    #if TEST_PART3
        printf("====================== PART3 ======================\n");
        test_isPalindrome();
        printf("\n");
    #endif
    
    #if TEST_PART4
        printf("====================== PART4 ======================\n");
        test_vectorOperations();
        printf("\n");
    #endif

    #if TEST_PART5
        printf("====================== PART5 ======================\n");
        test_gcd();
        printf("\n");
    #endif

        printf("===================== SUMMARY =====================\n");

    if (total_num_of_tests_passed == 0)
        printf(ANSI_COLOR_RED "All %d tests failed.\n\n" ANSI_COLOR_RESET, total_num_of_tests);
    else if (total_num_of_tests == total_num_of_tests_passed)
		printf(ANSI_COLOR_GREEN "All %d tests passed.\n\n" ANSI_COLOR_RESET, total_num_of_tests);
    else
		printf(ANSI_COLOR_YELLOW "%d out of %d tests passed.\n\n" ANSI_COLOR_RESET, 
		       total_num_of_tests_passed, total_num_of_tests);
        
    getchar();
    return 0;
}

/****************************************************************************************/

void changeCase_c(char *string)
{
    char ch;
    int c = 0;
    while (string[c] != '\0') {
        ch = string[c];
        if (ch >= 'A' && ch <= 'Z')
            string[c] += 'a' - 'A';
        else if (ch >= 'a' && ch <= 'z')
            string[c] -= 'a' - 'A';
        c++;
    }
}

void test_changeCase()
{
    int num_of_testcases = 5;
    char input_strings[100][1000] = { "ThisIsTheFinalLabOFics51", "iAMonAseafoodDIET.IseeFOODandIeatIT", 
		                              "l============!============l",
                                      "!q2#w4%e6...() -= TheDonald", 
									  "abcdefghijklmnopqrstuvwxyz{0123456789}ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
    char student_strings[100][1000] = { "ThisIsTheFinalLabOFics51", "iAMonAseafoodDIET.IseeFOODandIeatIT", 
		                                "l============!============l",
                                        "!q2#w4%e6...() -= TheDonald", 
										"abcdefghijklmnopqrstuvwxyz{0123456789}ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
    char golden_strings[100][1000] = { "ThisIsTheFinalLabOFics51", "iAMonAseafoodDIET.IseeFOODandIeatIT", 
		                               "l============!============l",
                                       "!q2#w4%e6...() -= TheDonald", 
									   "abcdefghijklmnopqrstuvwxyz{0123456789}ABCDEFGHIJKLMNOPQRSTUVWXYZ" };

    int err = 0;
    int i = 0;
    for (i = 0; i < num_of_testcases; i++) {
        total_num_of_tests++;
        changeCase_c(golden_strings[i]);
        changeCase(student_strings[i]);
        if (strcmp(golden_strings[i], student_strings[i]) != 0) {
            err = 1;
            printf(ANSI_COLOR_RED "\nTest failed. changeCase(%s): \nexpected %s \ngot      %s\n" ANSI_COLOR_RESET, 
                   input_strings[i], golden_strings[i], student_strings[i]);
        }
        else {
            total_num_of_tests_passed++;
        }
    }

    if (!err) {
        printf(ANSI_COLOR_GREEN "All tests passed.\n" ANSI_COLOR_RESET);
    }
}

/****************************************************************************************/

int findMinIndex_c (int integer_array[], int i, int j)
{
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
}

void selectionSort_c(int integer_array[], int array_size)
{
    int j;
    int iMin;
    int temp;

    // advance the position through the entire array //
    // (could do j < n-1 because single element is also min element) //
    for (j = 0; j < array_size-1; j++) {

        // find the index of min element in the unsorted a[j .. n-1] //
        iMin = findMinIndex_c (integer_array, j, array_size);
 
        if(iMin != j) { // swap values
            temp = integer_array[iMin];
            integer_array[iMin] = integer_array [j];
            integer_array[j] = temp;    
        }
    }
}

int compare_arrays(int a[], int b[], int len) 
{
    for (int ii = 1; ii <= len; ii++) {
        if (a[ii] != b[ii]) 
            return 0;
    }
    return 1;
}

void test_selectionSort()
{
    int num_of_testcases = 7;
    int student_arrayOfNumbers[100][100] ={ 
                                          { 4, 5, 1, 222, 5, 5, 33, 11, -4, 12, -333, 10, -28349, 74, 323232 },
                                          { +32131, +432, +232, +4, -1, -4, -5, -7, -8, -8, -9, -100000, -200000, -300000, -9999999 },
                                          { -90, 1, -800, 6, -1000, 0, 0, 0, -9, 12, -1819 },
                                          { 4, -5, 1, -222, 5, -5, 33, -11, -4, 12, -333, 10, +28349, 74, -323232 },
                                          { 100 },
                                          { -10, -100 }, 
                                          { 666, 666, 666, 666, 666, 666, 666, -100, -100 }
                                          };

    int golden_arrayOfNumbers[100][100] = { 
                                          { 4, 5, 1, 222, 5, 5, 33, 11, -4, 12, -333, 10, -28349, 74, 323232 },
                                          { +32131, +432, +232, +4, -1, -4, -5, -7, -8, -8, -9, -100000, -200000, -300000, -9999999 },
                                          { -90, 1, -800, 6, -1000, 0, 0, 0, -9, 12, -1819 },
                                          { 4, -5, 1, -222, 5, -5, 33, -11, -4, 12, -333, 10, +28349, 74, -323232 },
                                          { 100 },
                                          { -10, -100 }, 
                                          { 666, 666, 666, 666, 666, 666, 666, -100, -100 }
                                          };

	int len[100] = { 15, 15, 11, 15, 1, 2, 9 };

    int err = 0;
    int i;
    for (i = 0; i < num_of_testcases; i++) {
        total_num_of_tests++;
        selectionSort_c((golden_arrayOfNumbers[i]), len[i]);
        selectionSort((student_arrayOfNumbers[i]), len[i]);
        if (!compare_arrays(golden_arrayOfNumbers[i], student_arrayOfNumbers[i], len[i])) {
            err = 1;
            printf("\nCorrect output:\n");
            for (int j = 0; j<len[i]; j++){
                printf("%d ", golden_arrayOfNumbers[i][j]);
            }

            printf("\nYour output:\n");
            for (int j = 0; j<len[i]; j++){
                printf("%d ", student_arrayOfNumbers[i][j]);
            }
            printf("\n");
        }
        else {
            total_num_of_tests_passed++;
        }
    }

    if (!err) {
        printf(ANSI_COLOR_GREEN "All tests passed.\n" ANSI_COLOR_RESET);
    }
}

/****************************************************************************************/

int isPalindrome_c(char *input_string, int left_index, int right_index)
{
    /* Input Validation */
    if (NULL == input_string || left_index < 0 || right_index < 0){
        return -1;
    }
    /* Recursion termination condition */
    if (left_index >= right_index)
        return 1;
    if (input_string[left_index] == input_string[right_index]){
        return isPalindrome_c(input_string, left_index + 1, right_index - 1);
    }
    return -1;
}

void test_isPalindrome()
{
    int num_of_testcases = 7;
    char *inputs[] = { "7RCCR7", NULL, "CACC", "MAMAMAMAMAMAMAMAM" , 
		               "Was ItARatI saW", "neveroddoreven", "BON iver"};
    int len[] = {3, 0, 4, 17, 1};

    int err = 0;
    int i;
    for (i = 0; i < num_of_testcases; i++) {
        total_num_of_tests++;
        int golden_result = isPalindrome_c((inputs[i]), 0, len[i] - 1);
        int student_result = isPalindrome((inputs[i]), 0, len[i] - 1);
        if (golden_result != student_result) {
            err = 1;
            printf(ANSI_COLOR_RED "Test failed. isPalindrome(%s, %d, %d): expected %d, got %d\n" ANSI_COLOR_RESET, 
                   (inputs[i]), 0, len[i] - 1, golden_result, student_result);
        }
        else {
            total_num_of_tests_passed++;
        }
    }

    if (!err) {
        printf(ANSI_COLOR_GREEN "All tests passed.\n" ANSI_COLOR_RESET);
    }
}

/****************************************************************************************/

int performOperation_c (int operation, int first, int second)
{
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
}

void vectorOperations_c (int num_of_testcases, int *first_array, int *second_array, 
                         int *operation_array, int *output_array)
{
    int i;
    for (i=0; i<num_of_testcases; i++)
    {
        output_array[i] = performOperation_c(operation_array[i], first_array[i], second_array[i]);
    }
}

void test_vectorOperations()
{
    int err = 0;
    int i = 0;

    int num_of_testcases = 10;
    int operation_array[] = {1, 2, 1, 4, 1, 2, 4, 3, 3, 0};
    int first_array[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 182 };
    int second_array[] = {-3, 14, 5674, 289, - 89, 234, 54, 234, 98, 71} ;
    int student_output_array[10];
    int golden_output_array[10];

    vectorOperations_c(num_of_testcases, first_array, second_array, operation_array, golden_output_array);
	vectorOperations(num_of_testcases, first_array, second_array, operation_array, student_output_array);

    for (i=0; i<num_of_testcases; i++) {
        total_num_of_tests++;
        if (student_output_array[i] != golden_output_array[i]) {
            err = 1;
            printf(ANSI_COLOR_RED "Test failed. vectorOperations at index %d: expected %d, got %d\n" ANSI_COLOR_RESET, 
                   i, golden_output_array[i], student_output_array[i]);
        }
        else {
            total_num_of_tests_passed++;
        }
    }
    
    if (!err) {
		printf(ANSI_COLOR_GREEN "All tests passed.\n" ANSI_COLOR_RESET);
	}
}

/****************************************************************************************/

unsigned int gcd_c(unsigned int m, unsigned int n)
{
    if ((m % n) == 0)
        return n;
    else
        return gcd_c(n, m % n);
}

void test_gcd() 
{
    int num_of_testcases = 9;
    int first_array[] = { 10, 45, 832, 732, 1225 , 1, 27282, 9999, 101010 };
    int second_array[] = { 10, 20, 5619, 39, 70, 1, 162, 1111, 101010 };

    int err = 0;
    int i = 0;
    for (i = 0; i<num_of_testcases; i++) {
        total_num_of_tests++;
        unsigned int golden_result = gcd_c(first_array[i], second_array[i]);
        unsigned int student_result = gcd(first_array[i], second_array[i]);
        if (golden_result != student_result) {
            err = 1;
            printf(ANSI_COLOR_RED "Test failed. gcd(%u , %u): expected %u, got %u\n" ANSI_COLOR_RESET, 
                   first_array[i], second_array[i], 
                   gcd_c(first_array[i], second_array[i]), gcd(first_array[i], second_array[i]));
        }
        else {
            total_num_of_tests_passed++;
        }
    }

    if (!err) {
        printf(ANSI_COLOR_GREEN "All tests passed.\n" ANSI_COLOR_RESET);
    }
}