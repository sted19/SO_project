#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){

  // get from the PCB the id of the semaphore to open
  int sem_id = running->syscall_args[0];

  // ids are greater than 0
  if (sem_id < 0) {
    running->syscall_retvalue = DSOS_ESEMNEGID;
    return;
  }

  // check if semaphore id is already opened in the system, otherwise alloc the new semaphore
  Semaphore* s = SemaphoreList_byId(&semaphores_list ,sem_id);
  if (!s) {
    s = Semaphore_alloc(sem_id,1);
    List_insert(&semaphores_list, semaphores_list.last, (ListItem*)s);
  }
  else{
    //to remove
    printf("semaphore found and opened!\n");
  }

  // check if semaphore s has already been opened from this process
  SemDescriptor* aux = (SemDescriptor*) running->sem_descriptors.first;
  while (aux){
    if (aux->semaphore == s){
      running->syscall_retvalue = aux->fd;
      return;
    }
    aux = (SemDescriptor*)((ListItem*)aux)->next;
  }

  assert(!aux);

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
