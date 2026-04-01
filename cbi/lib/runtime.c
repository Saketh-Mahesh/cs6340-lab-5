/*
 * Copyright Â© 2021 Georgia Institute of Technology (Georgia Tech). All Rights Reserved.
 * Template code for CS 6340 Software Analysis
 * Instructors: Mayur Naik and Chris Poch
 * Head TAs: Kelly Parks and Joel Cooper
 *
 * Georgia Tech asserts copyright ownership of this template and all derivative
 * works, including solutions to the projects assigned in this course. Students
 * and other users of this template code are advised not to share it with others
 * or to make it available on publicly viewable websites including repositories
 * such as GitHub and GitLab. This copyright statement should not be removed
 * or edited. Removing it will be considered an academic integrity issue.
 *
 * We do grant permission to share solutions privately with non-students such
 * as potential employers as long as this header remains in full. However, 
 * sharing with other current or future students or using a medium to share
 * where the code is widely available on the internet is prohibited and 
 * subject to being investigated as a GT honor code violation.
 * Please respect the intellectual ownership of the course materials 
 * (including exam keys, project requirements, etc.) and do not distribute them 
 * to anyone not enrolled in the class. Use of any previous semester course 
 * materials, such as tests, quizzes, homework, projects, videos, and any other 
 * coursework, is prohibited in this course. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void __dbz_sanitizer__(int divisor, int line, int col) {
  if (divisor == 0) {
    printf("Divide-by-zero detected at line %d and col %d\n", line, col);
    exit(1);
  }
}

void __coverage__(int line, int col) {
  char exe[1024];
  int ret = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
  if (ret == -1) {
    fprintf(stderr, "Error: Cannot find /projc/self/exe\n");
    exit(1);
  }
  exe[ret] = 0;

  char logfile[1024];
  int len = strlen(exe);
  strncpy(logfile, exe, len);
  logfile[len] = 0;
  strcat(logfile, ".cov");
  FILE *f = fopen(logfile, "a");
  fprintf(f, "%d,%d\n", line, col);
  fclose(f);
}

void __cbi_branch__(int line, int col, int cond) {
  char exe[1024];
  int ret = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
  if (ret == -1) {
    fprintf(stderr, "Error: Cannot find /projc/self/exe\n");
    exit(1);
  }
  exe[ret] = 0;

  char logfile[1024];
  int len = strlen(exe);
  strncpy(logfile, exe, len);
  logfile[len] = 0;
  strcat(logfile, ".cbi");
  FILE *f = fopen(logfile, "a");
  fprintf(f, "branch,%d,%d,%d\n", line, col, cond);
  fclose(f);
}

void __cbi_return__(int line, int col, int rv) {
  char exe[1024];
  int ret = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
  if (ret == -1) {
    fprintf(stderr, "Error: Cannot find /projc/self/exe\n");
    exit(1);
  }
  exe[ret] = 0;

  char logfile[1024];
  int len = strlen(exe);
  strncpy(logfile, exe, len);
  logfile[len] = 0;
  strcat(logfile, ".cbi");
  FILE *f = fopen(logfile, "a");
  fprintf(f, "return,%d,%d,%d\n", line, col, rv);
  fclose(f);
}
