#include "semaphores_test.h"

// test sem_open
void test_semOpen(int id){
  int sem_fd = DisastrOS_semOpen(id);
  if (sem_fd<0) {
    fprintf(stderr,"error %d opening the semaphore with id %d\n",sem_fd,id);
    disastrOS_exit(sem_fd);
  }
  printf("opened semaphore with id=%d and descriptor=%d\n", id, sem_fd);
}

//test sem_close
void test_semClose(int id){
  int res = DisastrOs_semClose(id);
  if (res){
    fprintf(stderr,"error %d, semaphore %d does not exist!\n",res,id);
    disastrOS_exit(res);
  }
}

void test_semWait(int id){
    int res = DisastrOs_semWait(id);
    switch(res){
        case DSOS_ESEMNOTEX:
            fprintf(stderr,"semaphore does not exist!\n");
            return;
        case DSOS_ESEMNOTALLW:
            fprintf(stderr,"you must open the semaphore first!\n");
            return;
        default:
            return;
    }
}

void test_semPost(int id){
    int res = DisastrOs_semPost(id);
    switch(res){
        case DSOS_ESEMNOTEX:
            fprintf(stderr,"semaphore does not exist!\n");
            return;
        case DSOS_ESEMNOTALLW:
            fprintf(stderr,"you must open the semaphore first!\n");
            return;
        default:
            return;
    }
}
