/* Copyright 2016 Rose-Hulman
   But based on idea from http://cnds.eecs.jacobs-university.de/courses/caoslab-2007/
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


void handle_sigchld(int ignored)
{
  int status;
  wait(&status);
}

int main() {
  signal(SIGCHLD, handle_sigchld);
  char command[82];
  char *parsed_command[2];
  //takes at most two input arguments
  // infinite loop but ^C quits
  while (1) {
    printf("SHELL%% ");
    fgets(command, 82, stdin);
    command[strlen(command) - 1] = '\0';//remove the \n
    int len_1;
    for(len_1 = 0;command[len_1] != '\0';len_1++){
      if(command[len_1] == ' ')
        break;
    }
    parsed_command[0] = command;
    if(len_1 == strlen(command)){
      printf("Command is '%s' with no arguments\n", parsed_command[0]); 
      parsed_command[1] = NULL;

      // leave this here, do not change it
      if(!strcmp(parsed_command[0], "quit") ||
         !strcmp(parsed_command[0], "exit")) {
        exit(EXIT_SUCCESS);
      }
    }else{
      command[len_1] = '\0';
      parsed_command[1] = command + len_1 + 1;

      // leave this here, do not change it
      if(!strcmp(parsed_command[0], "quit") ||
         !strcmp(parsed_command[0], "exit")) {
        exit(EXIT_SUCCESS);
      }
    }
    if (strlen(parsed_command[0]) > 1) {
      if (parsed_command[0][0] == 'B' && parsed_command[0][1] == 'G') {
        parsed_command[0] = parsed_command[0] + 2;
        pid_t pid = fork();
        if (pid == 0) {
          pid_t pid2 = fork();
          if (pid2 == 0) {
            execlp(parsed_command[0], parsed_command[0], parsed_command[1], NULL);
            printf("BG command finished\n");
            exit(0);
          }
          else {
            int status;
            wait(&status);
            exit(SIGCHLD);
          }
        }
      }
      else {
        execlp(parsed_command[0], parsed_command[0], parsed_command[1], NULL);
      }
    }
    else {
      execlp(parsed_command[0], parsed_command[0], parsed_command[1], NULL);
    }

  }
}
