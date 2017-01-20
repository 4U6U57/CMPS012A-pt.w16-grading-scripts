// Compiles to a script which can be run per person to read the errors file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void printFile(FILE *fp) {
  char buf[1001];
  while (fgets(buf, 1000, fp)) {
    printf("%s", buf);
  }
}

int main() {
  char cwd[1024];
  if (!getcwd(cwd, sizeof(cwd)))
    perror("getcwd() error");
  FILE *check = fopen(".d.errors.f", "r");
  if (check) {
    fprintf(stdout, "Already graded %s\n", cwd);
    fclose(check);
    return 0;
  }
  char ded[10001]; // contains an array of all deduction comments
  ded[0] = '\0';
  char fname[51];
  sprintf(fname, "errors.temp");
  int grade = 12; // default
  FILE *in = fopen(fname, "r");
  if (in) {
    printf("Here is %s's Errors:\n", cwd);
    printFile(in);
    fclose(in);
  } else {
    printf("Errors file doesn't exist for %s\nGrade defaulting to zero...\nAdding appropriate notes...\n", cwd);
    grade = 0;
    sprintf(ded, "Didn't submit an Errors file\n");
  }
  char input[1001];
  while (1) {
    printf("\nWhat would you like to do?\nHint: type '-l' for list of commands\n\n");
    fgets(input, 1000, stdin);
    printf("\n");
    input[strlen(input) - 1] = '\0';
    if (!strncmp(input, "-l", 3)) { // list commands
      printf("-l : list cmds\n-w : write deductions and exit\n-m : modify grade\n");
      printf("-c : check grade\n-p : reprint the errors file\n-n : change notes\n-s : skip temporarily\n");
      printf("-nh : list macros for use in notes\n");
    } else if (!strncmp(input, "-w", 3)) { // write files
      break;
    } else if (!strncmp(input, "-m", 3)) { // change grade and see grade and comments
      grade = -1;
      while (grade == -1) {
        int temp = 0;
        printf("How many points out of 12 would you like to give? ");
        fgets(input, 1000, stdin);
        input[strlen(input) - 1] = '\0';
        for (int i = 0; i < strlen(input); i++) {
          if (input[i] < '0' || input[i] > '9') {
            temp = -1;
            break;
          }
          temp *= 10;
          temp += ((int) input[i] - '0');
        }
        if (temp <= 12 && temp >= 0) {
          grade = temp;
        }
      }
      printf("Current grade is %d and current comments are:\n\n%s\n", grade, ded);
    } else if (!strncmp(input, "-c", 3)) { // check grade and comments
      printf("Current grade is %d and current comments are:\n\n%s\n", grade, ded);
    } else if (!strncmp(input, "-p", 3)) { // reprint the file
      in = fopen(fname, "r");
      if (!in) {
        printf("Errors file doesn't exist for %s\n", cwd);
      } else {
        printf("Here is %s's Errors:\n", cwd);
        printFile(in);
        fclose(in);
      }
    } else if (!strncmp(input, "-n", 3)) { // change notes
      printf("Type EOF character to finish adding notes\n");
      ded[0] = '\0';
      while(fgets(input, 1000, stdin)) {
        strncat(ded, input, strlen(input) + 1);
      }
      for (int i = 0; i < strlen(ded); i++) {
        if (ded[i] == '\\') {
          char tempded[1001];
          char tempded2[1001];
          tempded[0] = '\0';
          tempded2[0] = '\0';
          while (i < strlen(ded)) {
            for (int j = i; j < strlen(ded); j++) {
              ded[j] = ded[j + 1]; // it was intentional to do input[strlen(input) - 1] = input[strlen(input)] as this simply moves the null character one back
            }
            //ded[strlen(input - 1)] = '\0';
            if (ded[i] == ' ' || ded[i] == '\n') break;
            if (ded[i] == 'h') {
              strcat(tempded, "Forgot to mention error about hot being printed in two instances\n");
            } else if (ded[i] == 'b') {
              strcat(tempded, "Forgot to mention error where books was printed instead of book\n");
            } else if (ded[i] == 'l') {
              strcat(tempded, "Forgot to mention error where last weekend was printed instead of next weekend\n");
            } else if (ded[i] == 'p') {
              strcat(tempded, "Forgot to mention error where there was a missing period\n");
            } else if (ded[i] == 'i') {
              strcat(tempded, "Forgot to mention error about invalid sentence codes printing as valid\n");
            } else if (ded[i] == 'f') {
              strcat(tempded, "Forgot to mention error about valid being printed on sentence codes below 1 and those above 5\n");
            } else if (ded[i] == 'g') {
              strcat(tempded, "An error is strictly defined to be a deviation from expected output, any other explanation is incorrect\n");
            }
          }
          sprintf(tempded2, "%s", ded + i); // pointer arithmetic =D
          ded[i] = '\0';
          strcat(ded, tempded);
          strcat(ded, tempded2);
        }
      }
      printf("\n\nCurrent grade is %d and current comments are:\n\n%s\n", grade, ded);
    } else if (!strncmp(input, "-s", 3)) { // skip temporarily
      printf("Skipping %s temporarily\n", cwd);
      return 0;
    } else if (!strncmp(input, "-nh", 4)) {
      printf("Notes auto replace a macro set with relevant generic comments informing the student that they did something wrong.\n");
      printf("Note: In the actual macro you would not put the '[' or ']' and a space or newline ends the macro\nHere are all possible macros:\n\\[hblpifg]\n");
      printf("h: cold->hot\nb: book->books\nl: next->last\np: period missing\ni: didn't mention invalid sentence codes\n");
      printf("f: mentioned only half the range of invalid sentence codes\ng: At least one instance of incorrect reasoning for an error\n");
      printf("<anything else>: will be ignored\n");
    } else { // system call
      //printf("Invalid comamand, type '-l' for a list of commands\n");
      system(input);
    }
  }
  FILE *out = fopen(".d.errors.f", "w");
  fprintf(out, "%d/12 points%s\n", grade, strlen(ded) ? " - See Error Notes" : " - No Notes");
  fclose(out);
  if (strlen(ded)) {
    FILE *notes = fopen(".notes.f", "a");
    fprintf(notes, "\n\nError Notes:\n==========\n\n%s\n==========\n\n", ded);
    fclose(notes);
  }
  printf("Wrote grade %d/12 for %s\n", grade, cwd);
  return 0;
}
