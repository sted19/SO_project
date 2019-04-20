#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "semaphores_test.h"
#include "disastrOS.h"
#include <assert.h>

int global_var = 0;

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");

  // my semOpen tes
  int sem_fd = test_semOpen(5);
  int sem_fd2 = test_semOpen(5);
  assert(sem_fd == sem_fd2);

  // my semWait test
  test_semWait(sem_fd);
  test_semWait(sem_fd+1);

  fprintf(stdout,"Process %d\tglobal var is %d\n",disastrOS_getpid(),global_var);
  global_var++;
  fprintf(stdout,"Process %d\tglobal var is %d\n",disastrOS_getpid(),global_var);
  disastrOS_sleep(10);
  fprintf(stdout,"Process %d\tglobal var is %d\n",disastrOS_getpid(),global_var);

  test_semPost(sem_fd);
  test_semPost(sem_fd-1);
  
  printf("PID: %d, terminating\n", disastrOS_getpid());

  for (int i=0; i<(disastrOS_getpid()+1); ++i){
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*5);
  }

  test_semClose(sem_fd);
  test_semClose(sem_fd-1);

  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  

  printf("I feel like to spawn 4 nice threads\n");
  int alive_children=0;
  int sem_fd[4];
  for (int i=0; i<4; ++i) {
    sem_fd[i] = test_semOpen(i);

    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    disastrOS_printStatus();
    --alive_children;
  }

  // close all semaphores previously opened
  for (int i=0; i<4;i++){
    test_semClose(sem_fd[i]);
  }
  disastrOS_printStatus();

  printf("shutdown!");
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}