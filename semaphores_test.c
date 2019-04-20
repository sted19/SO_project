#include "semaphores_test.h"

// test sem_open
int test_semOpen(int id){
  int sem_fd = DisastrOS_semOpen(id);
  switch (sem_fd){
        case DSOS_ESEMNEGID:
            fprintf(stderr,"SEM_OPEN: semaphore ids must be >= 0!\n");
            return -1;
        case DSOS_ESEMNOFD:
            fprintf(stderr,"SEM_OPEN: error during alloc of file descriptor memory block\n");
            return -1;
        default:
            printf("SEM_OPEN: opened semaphore with id=%d and descriptor=%d\n", id, sem_fd);
            return sem_fd;
  }
  
}

//test sem_close
void test_semClose(int fd){
  int res = DisastrOs_semClose(fd);
  switch(res){
        case DSOS_ESEMNEGFD:
            fprintf(stderr,"SEM_CLOSE: file descriptors must be >= 0!\n");
            return;
        case DSOS_ESEMNOTALLW:
            fprintf(stderr,"SEM_CLOSE: you must open the semaphore first!\n");
            return;
        default:
            return;
    }
}

void test_semWait(int fd){
    int res = DisastrOs_semWait(fd);
    switch(res){
        case DSOS_ESEMNEGFD:
            fprintf(stderr,"SEM_WAIT: file descriptors must be >= 0!\n");
            return;
        case DSOS_ESEMNOTALLW:
            fprintf(stderr,"SEM_WAIT: you must open the semaphore first!\n");
            return;
        default:
            return;
    }
}

void test_semPost(int fd){
    int res = DisastrOs_semPost(fd);
    switch(res){
        case DSOS_ESEMNEGFD:
            fprintf(stderr,"SEM_POST: file descriptors must be >= 0!\n");
            return;
        case DSOS_ESEMNOTALLW:
            fprintf(stderr,"SEM_POST: you must open the semaphore first!\n");
            return;
        default:
            return;
    }
}
