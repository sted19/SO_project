#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  
  int sem_id = running->syscall_args[0];

  // ids are greater than 0
  if (sem_id < 0) {
    running->syscall_retvalue = DSOS_ESEMNEGID;
    return;
  }

  // check if semaphore id is already opened in the system
  Semaphore* s = SemaphoreList_byId(&semaphores_list ,sem_id);
  if (!s) {
    running->syscall_retvalue = DSOS_ESEMNOTEX;
    return;
  }

  // check wheter the semaphore is amongst the process' open descriptors
  ListItem* aux = running->sem_descriptors.first;
  int present = 0;
  for (int i = 0; i<running->sem_descriptors.size; i++){
    if (((SemDescriptor*)aux)->semaphore == s) {
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

  if (s->count > 0){
    s->count--;
    running->syscall_retvalue=0;
    return;
  }

  assert(s->count==0);

  SemDescriptorPtr* d_ptr = SemDescriptorPtr_alloc(((SemDescriptor*)aux));

  // insert the descriptor pointer associated with the process amongst the waiting descriptors of the semaphore
  List_insert(&s->waiting_descriptors,s->waiting_descriptors.last, (ListItem*)d_ptr);

  // put in waiting status and start next process
  running->status = Waiting;
  List_insert(&waiting_list, waiting_list.last, (ListItem*) running);

  running->syscall_retvalue = 0;

  // pick the next
  PCB* next_running= (PCB*) List_detach(&ready_list, ready_list.first);
  running=next_running;
}
