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

  printf("Now I open a lot of semaphore to test if there is the right error code if I open too many of them\n");
  int fd_tmp;
  for (int i = 0; i<MAX_NUM_DESCRIPTORS_PER_PROCESS+1; i++){
    fd_tmp = test_semOpen(i);
  }
  printf("The last descriptor returned is %d\n",fd_tmp);
  printf("now i close all these semaphores\n");
  for (int i = 0; i<MAX_NUM_DESCRIPTORS_PER_PROCESS; i++){
    test_semClose(i);
  }

  printf("Open semaphore with id=5\n");
  int sem_fd = test_semOpen(5);
  printf("Open twice the semaphore 5, the print should return the same fd=%d as before\n",sem_fd);
  int sem_fd2 = test_semOpen(5);
  printf("%d==%d\n",sem_fd,sem_fd2);
  

  printf("SemWait on fd=%d\n",sem_fd);
  test_semWait(sem_fd);
  printf("SemWait on fd=%d, should return DSOS_ESEMNOTALLW error code %d\n",sem_fd+1,DSOS_ESEMNOTALLW);
  test_semWait(sem_fd+1);

  printf("THREAD %d HAS ENTERED CRITICAL SECTION\n", disastrOS_getpid());

  fprintf(stdout,"Thread %d\tglobal var is %d\n",disastrOS_getpid(),global_var);
  global_var++;
  fprintf(stdout,"Thread %d\tglobal var is %d\tthis thread will now sleep for a while\n",disastrOS_getpid(),global_var);
  disastrOS_sleep(20);
  fprintf(stdout,"Thread %d is out of sleep\tglobal var is %d\n",disastrOS_getpid(),global_var);

  printf("THREAD %d WILL NOW EXIT CRITICAL SECTION\n", disastrOS_getpid());

  printf("SemPost on fd=%d\n",sem_fd);
  test_semPost(sem_fd);
  printf("SemPost on fd=%d, should return DSOS_ESEMNOTALLW error code %d\n",sem_fd+1,DSOS_ESEMNOTALLW);
  test_semPost(sem_fd+1);
  
  printf("PID: %d, terminating\n", disastrOS_getpid());

  for (int i=0; i<(disastrOS_getpid()+1); ++i){
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*5);
  }

  printf("close semaphore with fd=%d\n",sem_fd);
  test_semClose(sem_fd);
  printf("try to close semaphore with fd=%d, should return DSOS_ESEMNOTALLW error code %d\n",sem_fd+1,DSOS_ESEMNOTALLW);
  test_semClose(sem_fd+1);

  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  

  printf("I feel like to spawn 4 nice threads\n");
  int alive_children=0;

  printf("main thread tries to open semaphore with id<0, should return DSOS_ESEMNEGID error code %d\n",DSOS_ESEMNEGID);
  int sem_fdo = test_semOpen(-1);
  printf("error code returned is %d\n",sem_fdo);

  printf("main thread opens 4 semaphores\n");
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

  printf("main thread closes definitely all semaphores\n");
  for (int i=0; i<4;i++){
    test_semClose(sem_fd[i]);
  }

  printf("main thread tries to close semaphore with fd<0, should return DSOS_ESEMNEGFD error code %d\n",DSOS_ESEMNEGFD);
  test_semClose(-1);

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