#ifndef diff4_h
#define diff4_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>

char buf[BUFLEN];
char *strings1[MAXSTRINGS], *strings2[MAXSTRINGS];
int showversion = 0, showbrief = 0, ignorecase = 0, report_identical = 0, showsidebyside = 0;
int showleftcolumn = 0, showunified = 0, showcontext = 0, suppresscommon = 0, diffnormal = 0;

int count1 = 0, count2 = 0, different = 0;
const char* files[2] = { NULL, NULL };
void version(void);
void todo_list(void);
void print_option(const char* name, int value);
void loadfiles(const char* filename1, const char* filename2);
void showoptions(const char* file1, const char* file2);
void setoption(const char* arg, const char* s, const char* t, int* value);
void diff_output_conflict_error(void);
void init_options_files(int argc, const char* argv[]);
void sidebyside(para* p, para* q, para* qlast, int foundmatch, int *differ[], char which, int common);
void context(para* p, para* q, para* qlast, int common);


#endif