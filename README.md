# SO_project
Semaphores implementation in disastrOS. This project is being developed for the exam of SO.

## Implemented functions:

- int disastrOs_semOpen(int sem_id)
- int disastrOs_semWait(int fd)
- int disastrOs_semPost(int fd)
- int disastrOs_semClose(int fd)

## Instructions to run

disastrOs_test.c file is used to try and test the implementation. To run the test, first execute "./make" command in the directory of the project, then run "./disastrOs_test".

## disastrOs_semOpen

### Description

**disastrOs_semOpen()** creates a new disastrOs semaphore or opens an existing semaphore. The semaphore is identified by its id.

### Return value

On Success, **disastrOs_semOpen()** returns the file descriptor number associated with the new semaphore. This number will be used when calling other disastrOs semaphore-related functions. On error, **disastrOs_semOpen()** returns an error code depending on what caused the error:

- **DSOS_ESEMNEGID**      the id provided is lower than 0
- **DSOS_ESEMNENOFD**     there has been an error allocating the file descriptor's memory block
- **DSOS_ESEMNENOFDPTR**  there has been an error allocating the file descriptor pointer's memory block
- **DSOS_ESEMNOALLOC**    there has been an error allocating the semaphore's memory block
- **DSOS_ESEMMAXNUMDES**  maximum number of file descriptors per process has been reached

## disastrOs_semWait

### Description

**disastrOs_semWait()** decrements the count variable of the semaphore associated with the file descriptor it takes as input. If the count variable has value greater than zero, then the decrement proceeds, and the function returns immediately. If the semaphore currently has the value zero, then the calling process is inserted in the semaphore's waiting list, his status is set to "waiting" and the next process available will start running.

### Return value

On Success, **disastrOs_semWait()** returns 0, on error instead it returns an error code depending on what caused the error:

- **DSOS_ESEMNEGFD**      the file descriptor provided is lower than 0
- **DSOS_ESEMNENOFDPTR**  there has been an error allocating the file descriptor pointer's memory block
- **DSOS_ESEMNOALLW**     the semaphore has not been previously opened or has already been closed

## disastrOs_semPost

### Description

**disastrOs_semPost()** increments the count variable of the semaphore associated with the file descriptor it takes as input. If the count variable has value greater than zero, then the increment proceeds, and the function returns immediately. If the semaphore currently has the value zero, then the first waiting process in the semaphore's waiting list is inserted in the ready list and its status is set to "ready".

### Return value

On Success, **disastrOs_semPost()** returns 0, on error instead it returns an error code depending on what caused the error:

- **DSOS_ESEMNEGFD**      the file descriptor provided is lower than 0
- **DSOS_ESEMNOALLW**     the semaphore has not been previously opened or has already been closed

## disastrOs_semClose

### Description

**disastrOs_semClose()** closes the semaphore referred to by fd for the calling process. If the calling process is the only one remaining amongst the processes that opened the semaphore, then any resources that the system had allocated for this semaphore will be freed.

### Return value

On Success, **disastrOs_semClose()** returns 0, on error instead it returns an error code depending on what caused the error:

- **DSOS_ESEMNEGFD**      the file descriptor provided is lower than 0
- **DSOS_ESEMNOALLW**     the semaphore has not been previously opened or has already been closed
