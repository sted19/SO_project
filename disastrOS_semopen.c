#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){

  int sem_id = running->syscall_args[0];

  if (sem_id < 0) {
    fprintf(stderr,"Sem ID must be greater than 0!");
    running->syscall_retvalue = DSOS_ESEMNEGID;
    return;
  }

  Semaphore* s = SemaphoreList_byId(&semaphores_list ,sem_id);
  if (!s) {
    s = Semaphore_alloc(sem_id,1);
    List_insert(&semaphores_list, semaphores_list.last, (ListItem*)s);
  }
  else{
    printf("semaphore found, will open it!\n");
  }

  SemDescriptor* fd = SemDescriptor_alloc(running->last_sem_fd, s, running);
  if (!fd) {
    running->syscall_retvalue = DSOS_ESEMNOFD;
    return;
  }  
  running->last_sem_fd++;

  SemDescriptorPtr* fd_ptr = SemDescriptorPtr_alloc(fd);

  List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*)fd);
  
  fd->ptr = fd_ptr;
  List_insert(&s->descriptors, s->descriptors.last, (ListItem*)fd_ptr);

  running->syscall_retvalue = fd->fd;
}
