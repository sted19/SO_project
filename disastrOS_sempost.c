#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  int sem_id = running->syscall_args[0];

  // ids are greater than 0
  if (sem_id < 0) {
    running->syscall_retvalue = DSOS_ESEMNEGID;
    return;
  }

  // check if semaphore id is already opened in the system, otherwise alloc the new semaphore
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

  if (s->count>0){
    s->count++;
    running->syscall_retvalue = 0;
    return;
  }

  assert(s->count==0);

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
