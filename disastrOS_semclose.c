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
  SemDescriptor* aux = (SemDescriptor*)SemDescriptorList_byFd((ListHead*)&running->sem_descriptors,fd);
  if (!aux){
    running->syscall_retvalue = DSOS_ESEMNOTALLW;
    return;
  }

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
