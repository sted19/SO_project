#include "semaphores_test.h"

// test sem_open
int test_semOpen(int id){
  int sem_fd = disastrOs_semOpen(id);
  switch (sem_fd){
        case DSOS_ESEMNEGID:
            fprintf(stderr,"SEM_OPEN ERROR %d: semaphore ids must be >= 0!\n",DSOS_ESEMNEGID);
            return -1;
        case DSOS_ESEMNOFD:
            fprintf(stderr,"SEM_OPEN ERROR %d: error allocating file descriptor's memory block\n",DSOS_ESEMNOFD);
            return -1;
        case DSOS_ESEMNOALLOC:
            fprintf(stderr,"SEM_OPEN ERROR %d: error allocating semaphore's memory block\n",DSOS_ESEMNOALLOC);
            return -1;
        case DSOS_ESEMNOFDPTR:
            fprintf(stderr,"SEM_OPEN ERROR %d: error allocating file descriptor pointer's memory block\n",DSOS_ESEMNOFDPTR);
            return -1;
        case DSOS_ESEMMAXNUMDES:
            fprintf(stderr,"SEM_OPEN ERROR %d: allocated maximum number of file descriptors per process\n",DSOS_ESEMMAXNUMDES);
            return -1;
        default:
            printf("SEM_OPEN: opened semaphore with id=%d and descriptor=%d\n", id, sem_fd);
            return sem_fd;
  }
  
}

//test sem_close
void test_semClose(int fd){
  int res = disastrOs_semClose(fd);
  switch(res){
        case DSOS_ESEMNEGFD:
            fprintf(stderr,"SEM_CLOSE ERROR %d: file descriptors must be >= 0!\n",DSOS_ESEMNEGFD);
            return;
        case DSOS_ESEMNOTALLW:
            fprintf(stderr,"SEM_CLOSE ERROR %d: you must open the semaphore first!\n",DSOS_ESEMNOTALLW);
            return;
        default:
            return;
    }
}

void test_semWait(int fd){
    int res = disastrOs_semWait(fd);
    switch(res){
        case DSOS_ESEMNEGFD:
            fprintf(stderr,"SEM_WAIT ERROR %d: file descriptors must be >= 0!\n",DSOS_ESEMNEGFD);
            return;
        case DSOS_ESEMNOTALLW:
            fprintf(stderr,"SEM_WAIT ERROR %d: you must open the semaphore first!\n",DSOS_ESEMNOTALLW);
            return;
        case DSOS_ESEMNOFDPTR:
            fprintf(stderr,"SEM_WAIT ERROR %d: error allocating file descriptor pointer's memory block\n",DSOS_ESEMNOFDPTR);
        default:
            return;
    }
}

void test_semPost(int fd){
    int res = disastrOs_semPost(fd);
    switch(res){
        case DSOS_ESEMNEGFD:
            fprintf(stderr,"SEM_POST ERROR %d: file descriptors must be >= 0!\n",DSOS_ESEMNEGFD);
            return;
        case DSOS_ESEMNOTALLW:
            fprintf(stderr,"SEM_POST ERROR %d: you must open the semaphore first!\n",DSOS_ESEMNOTALLW);
            return;
        default:
            return;
    }
}
