#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  int fd = running->syscall_args[0];

  // fds are greater than 0
  if (fd < 0) {
    running->syscall_retvalue = DSOS_ESEMNEGFD;
    return;
  }

  // check whether the descriptor is amongst the process' open descriptors
  ListItem* aux = running->sem_descriptors.first;
  int present = 0;
  for (int i = 0; i<running->sem_descriptors.size; i++){
    if (((SemDescriptor*)aux)->fd == fd) {
      present = 1;
      break;
    }
    aux = aux->next;
  }
  // if the semaphore hasn't previously been opened, return NOT ALLOWED OPERATION error
  if (!present){
    running->syscall_retvalue=DSOS_ESEMNOTALLW;
    return;
  }

  assert(aux);
  Semaphore* s = ((SemDescriptor*)aux)->semaphore;

  // remove the descriptor from the list of the process' descriptors
  SemDescriptor* des = (SemDescriptor*)List_detach(&running->sem_descriptors, (ListItem*)aux);
  assert(des);

  SemDescriptorPtr* des_ptr = (SemDescriptorPtr*)des->ptr;
  des_ptr = (SemDescriptorPtr*)List_detach(&s->descriptors,(ListItem*)(des_ptr));

  assert(des_ptr);
  SemDescriptor_free(des);
  SemDescriptorPtr_free(des_ptr);

  if (s->descriptors.size == 0) {
    Semaphore* s_del = (Semaphore*)List_detach(&semaphores_list, (ListItem*)s);
    Semaphore_free(s_del);
  }

  running->syscall_retvalue=0;
}
