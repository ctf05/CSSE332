#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define simple_assert(message, test)                                           \
  do {                                                                         \
    if(!(test))                                                                \
      return message;                                                          \
  } while(0)
#define TEST_PASSED NULL
#define DATA_SIZE 100
#define INITIAL_VALUE 77
#define MAX_TESTS 10

/* test_funcs is an array of function pointers that store all of the test we
 * would like to run. */
char *(*test_funcs[MAX_TESTS])();

/* total number of tests registered */
static int num_tests = 0;

/* shared data that all the tests will use. */
static int data[DATA_SIZE][DATA_SIZE];

/**
 * Add a test to our list of test functions that we would like to run.
 *
 * @param test_func A pointer to the function that we would like to run.
 */
void add_test(char *(*test_func)()) {
  if(num_tests == MAX_TESTS) {
    printf("exceeded max possible tests");
    exit(1);
  }
  test_funcs[num_tests] = test_func;
  num_tests++;
}

/**
 * This setup function should run before each test
 */
void setup(void) {
  printf("starting setup\n");
  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      data[i][j] = INITIAL_VALUE;
    }
  }

  /* imagine this function does a lot of other complicated setup that takes a
   * long time. */
  usleep(3000000);
}

void alarmhand(int signal)
{ 
  exit(2);
}

/**
 * Run all the test in the test suite.
 */
void run_all_tests() {
  signal(SIGALRM,alarmhand);
  pid_t pid;
  pid_t pids[4];
  int pipes[4][1];
  setup();
  for (int i = 0; i < num_tests; i++) {
    //make a pipe between the parent and child
    pipe(pipes[i]);
    pid = fork();
    pids[i] = pid;
    if (pid == 0) {
      alarm(3);
      close(pipes[i][0]);
      // run the test based on the index
      char *message = test_funcs[i]();
      if(message == TEST_PASSED) {
        close(pipes[i][1]);
        exit(0);
      }
      write(pipes[i][1], message, strlen(message));
      close(pipes[i][1]);
      exit(1);
    }
    close(pipes[i][1]);
  }
  int status;
  int first;
  for (int i = 0; i < num_tests; i++) {
    waitpid(pids[i], &status, 0);
    if (WIFSIGNALED(status)) {
      printf("test Crashed");
    }
    else if (WIFEXITED(status)) {
      if (WEXITSTATUS(status) == 0) {
        printf("test Passed");
      }
      else if (WEXITSTATUS(status) == 1) {
        printf("test Failed");
      }
      else if (WEXITSTATUS(status) == 2) {
        printf("test Timed Out");
      }
      else {
        printf("problem in Code");
      }
    }
    first = 1;
    while (read(pipes[i][0], &status, 1) > 0) {
      if (first) {
        printf(": ");
        first = 0;
      }
      printf("%c", status);
    }
    printf("\n");
  }
}

char *test1() {
  printf("starting test 1\n");
  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      simple_assert("test 1 data not initialized properly",
                    data[i][j] == INITIAL_VALUE);
    }
  }

  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      data[i][j] = 1;
    }
  }

  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      simple_assert("test 1 data not set properly", data[i][j] == 1);
    }
  }
  printf("ending test 1\n");
  return TEST_PASSED;
}

char *test2() {
  printf("starting test 2\n");
  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      simple_assert("test 2 data not initialized properly",
                    data[i][j] == INITIAL_VALUE);
    }
  }

  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      data[i][j] = 2;
    }
  }

  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      simple_assert("test 2 data not set properly", data[i][j] == 2);
    }
  }

  printf("ending test 2\n");
  return TEST_PASSED;
}

char *test3() {
  printf("starting test 3\n");

  simple_assert("test 3 always fails", 1 == 2);

  printf("ending test 3\n");
  return TEST_PASSED;
}

char *test4() {
  printf("starting test 4\n");

  int *val = NULL;
  printf("data at val is %d", *val);

  printf("ending test 4\n");
  return TEST_PASSED;
}

char *test5() {
  printf("starting test 5\n");

  while(1)
    ;

  printf("ending test 5\n");
  return TEST_PASSED;
}

int main(int argc, char **argv) {
  add_test(test1);
  add_test(test2);
  add_test(test3);
  add_test(test4);
  add_test(test5);
  run_all_tests();
}
