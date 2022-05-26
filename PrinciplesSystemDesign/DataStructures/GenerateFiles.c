#include <stdio.h>
#include <stdlib.h>

#define MAXCOURSES 10   //max number of courses
#define NUMOFQUARTERS 3 //number of quarters per year
#define COURSENAME "course"

int main(void)
{
    FILE* fout;
    char file1[] = "offering_tmp";
    char file2[] = "prerequisite_tmp";
    char file3[] = "requirement_tmp";

    char* fname = file1;
    fout = fopen(fname, "w");
    if (fout == NULL)
    {
        printf("Error of opening file %s\n", fname);
        exit(1);
    }

    for (int i = 0; i < MAXCOURSES; i++)
    {
        fprintf(fout, "%s%d %d\n", COURSENAME, i, (rand() % 3) + 1);
    }
    fclose(fout);

    fname = file2;
    fout = fopen(fname, "w");
    if (fout == NULL)
    {
        printf("Error of opening file %s\n", fname);
        exit(1);
    }

    //1st half
    int mid = (int)(MAXCOURSES / 2);
    for (int i = 0; i < mid; i++)
    {
        fprintf(fout, "%s%d ", COURSENAME, i);
        for (int j = i + 1; j < mid; j++)
            fprintf(fout, "%s%d ", COURSENAME, j);
        fprintf(fout, "\n");
    }
    //2nd half
    for (int i = mid; i < MAXCOURSES; i++)
    {
        fprintf(fout, "%s%d ", COURSENAME, i);
        for (int j = i + 1; j < MAXCOURSES; j++)
            fprintf(fout, "%s%d ", COURSENAME, j);
        fprintf(fout, "\n");
    }
    fclose(fout);

    fname = file3;
    fout = fopen(fname, "w");
    if (fout == NULL)
    {
        printf("Error of opening file %s\n", fname);
        exit(1);
    }
    fprintf(fout, "%s%d %s%d", COURSENAME, 0, COURSENAME, mid);
    fclose(fout);

    return 0;
}