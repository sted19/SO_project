#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  
  int fd = running->syscall_args[0];

  // fds are greater than 0
  if (fd < 0) {
    running->syscall_retvalue = DSOS_ESEMNEGFD;
    return;
  }

  // check whether the descriptor is amongst the process' open descriptors
  SemDescriptor* des = (SemDescriptor*)SemDescriptorList_byFd((ListHead*)&running->sem_descriptors,fd);
  if (!des){
    running->syscall_retvalue = DSOS_ESEMNOTALLW;
    return;
  }

  Semaphore* s = ((SemDescriptor*)des)->semaphore;

  // if count > 0, just need to decrement it
  if (s->count > 0){
    s->count--;
    running->syscall_retvalue=0;
    return;
  }

  assert(s->count==0);

  // if count == 0, need to put the process that is running now in the waiting lists and switch running process
  SemDescriptorPtr* d_ptr = SemDescriptorPtr_alloc(((SemDescriptor*)des));
  if (!d_ptr){
    running->syscall_retvalue = DSOS_ESEMNOFDPTR;
    return;
  }

  // insert the descriptor pointer associated with the process amongst the waiting descriptors of the semaphore
  List_insert(&s->waiting_descriptors,s->waiting_descriptors.last, (ListItem*)d_ptr);

  // put in waiting status and start next process
  running->status = Waiting;
  List_insert(&waiting_list, waiting_list.last, (ListItem*) running);

  running->syscall_retvalue = 0;

  PCB* next_running= (PCB*) List_detach(&ready_list, ready_list.first);
  running=next_running;
}
