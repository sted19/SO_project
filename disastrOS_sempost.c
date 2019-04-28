#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
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

  // if count > 0 means no-one is waiting, just need to increment it
  if (s->count>0){
    s->count++;
    running->syscall_retvalue = 0;
    return;
  }

  assert(s->count==0);

  // if count == 0, need to set as ready the first process that entered the waiting list
  if (s->waiting_descriptors.first){
    SemDescriptorPtr* des_ptr = (SemDescriptorPtr*)List_detach(&s->waiting_descriptors, s->waiting_descriptors.first);
    SemDescriptor* des = des_ptr->descriptor;
    PCB* first_in = des->pcb;
    assert(first_in->status == Waiting);
    List_detach(&waiting_list,(ListItem*)first_in);
    List_insert(&ready_list,ready_list.last,(ListItem*)first_in);
    first_in->status = Ready;

    // free memory allocated to create entry in s->waiting_descriptors
    SemDescriptorPtr_free(des_ptr);
  }
  else{
    s->count++;
  }
  running->syscall_retvalue = 0;
  return;
}
