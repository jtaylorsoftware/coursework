This code was originally for Lab 1 of ICS53 Winter 2018. The requirements were to generate a course plan
that enables a student to meet all course requirements and graduate, given three files containing the course
offerings per quarter, course prerequisites and the courses required to graduate. The course plan should also only contain
those courses that the student must take to meet all requirements.

My solution (`Lab1.c`) uses the reachability property of equivalence classes to generate a set of necessary courses to take, which ultimately excludes those courses that are not a prereq of a required course or itself a required course. It then creates and uses the prerequisites graph to generate the course plan from the set of necessary courses.

The implementation uses generic C++ data structures ported to C by using macros to generate type implementations.
The input `.txt` files with shorter names are quick to generate a course plan. The longer
file names take a while to run (they represent a harder problem) and my implementation
may run into memory issues. My solution does a lot of heap allocations and could probably be
more efficient in time + space, however, it does not leak memory according to `valgrind`.

The output from running with `of.txt pr.txt rq.txt` is:

```
1 1 l f p
1 2 a x y j
1 3 u d i
2 1 e
2 2 g
2 3 c
3 1 b
3 3 h
```
