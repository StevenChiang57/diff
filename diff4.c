//
//  diff_02.c
//  diff
//
//  Created by William McCarthy on 4/29/19.
//  Copyright © 2019 William McCarthy. All rights reserved.
// clang diff4.c para.c util.c -o test

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>


#define ARGC_ERROR 1
#define TOOMANYFILES_ERROR 2
#define CONFLICTING_OUTPUT_OPTIONS 3

#define MAXSTRINGS 1024
#define MAXPARAS 4096

#define HASHLEN 200
#define MAXLINE 100

#include "para.h"
#include "util.h"
#include "diff4.h"

int main(int argc, const char * argv[]) {
  init_options_files(--argc, ++argv);

//  para_printfile(strings1, count1, printleft);
//  para_printfile(strings2, count2, printright);

  para* p = para_first(strings1, count1);
  para* q = para_first(strings2, count2);
  int foundmatch = 0;
  bool printed = 0;
  para* qlast = q;
  para* plast = p;
  int* differ[MAXLINE];
  struct tm *fi1;
  struct tm *fi2;
  struct stat attrib1;
  struct stat attrib2;
  if(showsidebyside) sidebyside(p, q, qlast, foundmatch, differ, 'n', 1);
  else if (showleftcolumn) sidebyside(p, q, qlast, foundmatch, differ, '(', 1);
  else if (suppresscommon) sidebyside(p, q, qlast, foundmatch, differ, 'n', 0);
  else if (showcontext){
    stat(files[0], &attrib1);
    stat(files[1], &attrib2);
    fi1 = gmtime(&(attrib1.st_mtime));
    fi2 = gmtime(&(attrib2.st_mtime));
    printf("--- %s %d/%d(month)/%d(day) %d:%d:%d\n", files[0], fi1->tm_year + 1900, fi1->tm_mon + 1, fi1->tm_mday - 1, fi1->tm_hour, fi1->tm_min, fi1->tm_sec);
    printf("+++ %s %d/%d(month)/%d(day) %d:%d:%d\n", files[1], fi2->tm_year + 1900, fi2->tm_mon + 1, fi2->tm_mday - 1, fi2->tm_hour, fi2->tm_min, fi2->tm_sec);
    printf("*** ___,____ ****\n");
    context(p, q, qlast, 1);
  }
  else if (showunified){
    stat(files[0], &attrib1);
    stat(files[1], &attrib2);
    fi1 = gmtime(&(attrib1.st_mtime));
    fi2 = gmtime(&(attrib2.st_mtime));
    printf("--- %s %d/%d(month)/%d(day) %d:%d:%d\n", files[0], fi1->tm_year + 1900, fi1->tm_mon + 1, fi1->tm_mday - 1, fi1->tm_hour, fi1->tm_min, fi1->tm_sec);
    printf("+++ %s %d/%d(month)/%d(day) %d:%d:%d\n", files[1], fi2->tm_year + 1900, fi2->tm_mon + 1, fi2->tm_mday - 1, fi2->tm_hour, fi2->tm_min, fi2->tm_sec);
    printf("@@@ -___,____ +___,___ @@@\n");
    context(p, q, qlast, 0);
  }
  else{
  while (p != NULL) {
    qlast = q;
    plast = p;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;
    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        if((*q).secondline != NULL){ 
          whatfile = 2; 
          printf("%5d%c%d-%d\n", (*p).start , 'a' , (*q) .start + 1, (*q).stop + 1);
          }
        else {
          whatfile = 0;
          printf("%5d%c%d\n", (*p).start , 'a' , (*q).stop);
        }
        para_print(q, printnormal);
        q = para_next(q);
      }
      if(para_almostequal(p, q, differ, p->stop - p->start)){
        int dsa = 0;
        while(dsa != p->stop - p->start){
          if(differ[dsa] == 0){
            printf("%5dc%d\n", p->start + dsa + 1, q->start + dsa + 1);
            printf("< %s", p->base[p->start + dsa]);
            printf("----------\n");
            printf("> %s", q->base[q->start + dsa]);
          }
          ++dsa;
        }
      }
      p = para_next(p);
      if((*q).secondline != NULL && p != NULL && printed == 1 && !para_equal(p, q)){
        whatfile = 1;
        printf("%5d-%d%c%d\n", (*p).start, (*p).stop , ((*p).secondline == NULL) ? 'a' : 'd', (*q).stop);
      }
      else if ( p != NULL && printed == 1 && !para_equal(p, q)){ 
        whatfile = 0;
        printf("%5d%c%d\n", (*p).start + 1, ((*p).secondline == NULL) ? 'a' : 'd' , (*q).start);
      }
      q = para_next(q);
    } else {
      if((*p).secondline != NULL && q != NULL){ 
        whatfile = 1;
        printed = 1;
        printf("%5d-%d%c%d\n", (*p).start + 1, (*p).stop + 1, ((*p).secondline == NULL) ? 'a' : 'd', (*q).start);
        }
      else if (q!= NULL){
        whatfile = 0;
        printed = 1;
        printf("%5d%c%d\n", (*p).start + 1, ((*p).secondline == NULL) ? 'a' : 'd' , (*q).start);
      }
      para_print(p, printnormal);
      p = para_next(p);
    }
  }
  
  while (q != NULL) {
    if((*q).secondline != NULL){
      whatfile = 2;
      printf("%5d%c%d-%d\n", (*plast).stop , ((*q).secondline != NULL) ? 'a' : 'd', (*q).start, (*q).stop);
    }
    else{
      whatfile = 0;
      printf("%5d%c%d\n", (*plast).stop , ((*q).secondline != NULL) ? 'a' : 'd' , (*q).stop); 
      }
    para_print(q, printnormal);
    q = para_next(q);
    qlast = q;
  }
  }


  // while (p != NULL) {
  //   qlast = q;
  //   foundmatch = 0;
  //   while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
  //     q = para_next(q);
  //   }
  //   q = qlast;

  //   if (foundmatch) {
  //     while ((foundmatch = para_equal(p, q)) == 0) {
  //       para_print(q, printright);
  //       q = para_next(q);
  //       qlast = q;
  //     }
  //     para_print(q, printboth);
  //     p = para_next(p);
  //     q = para_next(q);
  //   } else {
  //     para_print(p, printleft);
  //     p = para_next(p);
  //   }
  // }
  // while (q != NULL) {
  //   para_print(q, printright);
  //   q = para_next(q);
  // }

  return 0;
}

void version(void) {
  printf("\n\n\ndiff (CSUF diffutils) 1.0.0\n");
  printf("Copyright (C) 2014 CSUF\n");
  printf("This program comes with NO WARRANTY, to the extent permitted by law.\n");
  printf("You may redistribute copies of this program\n");
  printf("under the terms of the GNU General Public License.\n");
  printf("For more information about these matters, see the file named COPYING.\n");
  printf("Written by William McCarthy, Tony Stark, and Dr. Steven Strange\n\n");
}

void todo_list(void) {
  printf("\n\n\nTODO: check line by line in a paragraph, using '|' for differences");
  printf("\nTODO: this starter code does not yet handle printing all of fin1's paragraphs.");
  printf("\nTODO: handle the rest of diff's options\n");
}

void loadfiles(const char* filename1, const char* filename2) {
  memset(buf, 0, sizeof(buf));
  memset(strings1, 0, sizeof(strings1));
  memset(strings2, 0, sizeof(strings2));
  
  FILE *fin1 = openfile(filename1, "r");
  FILE *fin2 = openfile(filename2, "r");
  
  while (!feof(fin1) && fgets(buf, BUFLEN, fin1) != NULL) { strings1[count1++] = strdup(buf); }  fclose(fin1);
  while (!feof(fin2) && fgets(buf, BUFLEN, fin2) != NULL) { strings2[count2++] = strdup(buf); }  fclose(fin2);
  
  if (count1 != count2) { different = 1;  return; }
  for (int i = 0, j = 0; i < count1 && j < count2;  ++i, ++j) {
    if (strcmp(strings1[i], strings2[j]) != 0) { different = 1;  return; }
  }
}

void print_option(const char* name, int value) { printf("%17s: %s\n", name, yesorno(value)); }

void diff_output_conflict_error(void) {
  fprintf(stderr, "diff: conflicting output style options\n");
  fprintf(stderr, "diff: Try `diff --help' for more information.)\n");
  exit(CONFLICTING_OUTPUT_OPTIONS);
}

void setoption(const char* arg, const char* s, const char* t, int* value) {
  if ((strcmp(arg, s) == 0) || ((t != NULL && strcmp(arg, t) == 0))) {
    *value = 1;
  }
}

void showoptions(const char* file1, const char* file2) {
  printf("diff options...\n");
  print_option("diffnormal", diffnormal);
  print_option("show_version", showversion);
  print_option("show_brief", showbrief);
  print_option("ignorecase", ignorecase);
  print_option("report_identical", report_identical);
  print_option("show_sidebyside", showsidebyside);
  print_option("show_leftcolumn", showleftcolumn);
  print_option("suppresscommon", suppresscommon);
  print_option("showcontext", showcontext);
  print_option("show_unified", showunified);
  
  printf("file1: %s,  file2: %s\n\n\n", file1, file2);
  
  printline();
}

void init_options_files(int argc, const char* argv[]) {
  int cnt = 0;

  
  while (argc-- > 0) {
    const char* arg = *argv;
    setoption(arg, "-v",       "--version",                  &showversion);
    setoption(arg, "-q",       "--brief",                    &showbrief);
    setoption(arg, "-i",       "--ignore-case",              &ignorecase);
    setoption(arg, "-s",       "--report-identical-files",   &report_identical);
    setoption(arg, "--normal", NULL,                         &diffnormal);
    setoption(arg, "-y",       "--side-by-side",             &showsidebyside);
    setoption(arg, "--left-column", NULL,                    &showleftcolumn);
    setoption(arg, "--suppress-common-lines", NULL,          &suppresscommon);
    setoption(arg, "-c",       "--context",                  &showcontext);
    setoption(arg, "-u",       "showunified",                &showunified);
    if (arg[0] != '-') {
      if (cnt == 2) {
        fprintf(stderr, "apologies, this version of diff only handles two files\n");
        fprintf(stderr, "Usage: ./diff [options] file1 file2\n");
        exit(TOOMANYFILES_ERROR);
      } else { files[cnt++] = arg; }
    }
    ++argv;   // DEBUG only;  move increment up to top of switch at release
  }

  if (!showcontext && !showunified && !showsidebyside && !showleftcolumn) {
    diffnormal = 1;
  }
  
  if (showversion) { version();  exit(0); }
  
  if (((showsidebyside || showleftcolumn) && (diffnormal || showcontext || showunified)) ||
      (showcontext && showunified) || (diffnormal && (showcontext || showunified))) {

    diff_output_conflict_error();
  }
  
//  showoptions(files[0], files[1]);
  loadfiles(files[0], files[1]);

  if (report_identical && !different) { printf("The files are identical.\n\n");   exit(0); }

  if (showbrief && different) { printf("The files are different.\n\n");   exit(0); }
}

// ./test -y left.txt right.txt 
// ./test –-left-column left.txt right.txt 
// ./test --suppress-common-lines left.txt right.txt 
void sidebyside(para* p, para* q, para* qlast, int foundmatch, int *differ[], char which, int common){
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print(q, printright);
        q = para_next(q);
        qlast = q;
      }
      if(para_almostequal(p, q, differ, p->stop - p->start)){
        int dsa = 0;
        while(dsa != p->stop - p->start){
          int temp = strlen(p->base[p->start + dsa]);
          p->base[p->start + dsa][temp - 1] = 0;
          if(differ[dsa] == 0) printf("%-48s | %s", p->base[p->start + dsa], q->base[q->start + dsa]);
          else if(common == 1){
            if(which == 'n') printf("%-49s  %s", p->base[p->start + dsa], q->base[q->start + dsa]);
            else if(which == '(') printf("%-47s  %c\n", p->base[p->start + dsa], which);
          }   
          ++dsa;
        }
      }
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print(p, printleft);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print(q, printright);
    q = para_next(q);
  }

}

// ./test -c left.txt right.txt 
void context(para* p, para* q, para* qlast, int common){
  int foundmatch = 0;
  while (p != NULL) {
    qlast = q;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;
    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        whatfile = 2; 
        para_print(q, printnormalplus);
        q = para_next(q);
      }
      if (common == 1) para_print(q, printnormal2);
      p = para_next(p);
      q = para_next(q);
    } else {
      whatfile = 1;
      para_print(p, printnormalplus);
      p = para_next(p);
    }
  }
  
  while (q != NULL) {
    whatfile = 2;
    para_print(q, printnormalplus);
    q = para_next(q);
    qlast = q;
  }
}

