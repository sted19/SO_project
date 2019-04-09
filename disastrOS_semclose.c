#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  // retrieve the id of the semaphore to close from the pcb of the running process
  int sem_id = running->syscall_args[0];

  // check wheter the semaphore exists or not
  Semaphore* s = SemaphoreList_byId(&semaphores_list ,sem_id);
  if (!s) {
    running->syscall_retvalue=DSOS_ESEMNOTEX;
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
  if (present){
    assert(aux);

    // remove the descriptor from the list of the process' descriptors
    SemDescriptor* des = (SemDescriptor*)List_detach(&running->sem_descriptors, (ListItem*)aux);
    assert(des);

    SemDescriptorPtr* des_ptr = (SemDescriptorPtr*)des->ptr;
    des_ptr = (SemDescriptorPtr*)List_detach(&s->descriptors,(ListItem*)(des_ptr));

    assert(des_ptr);
    SemDescriptor_free(des);
    SemDescriptorPtr_free(des_ptr);
  }

  if (s->descriptors.size == 0) Semaphore_free(s);

  running->syscall_retvalue=0;
}
