// STOLE THIS FILE FROM KAZAA, FUCK A HARVARD UNIVERSITY
#include <types.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/wait.h>
#include <lib.h>
#include <syscall.h>
#include <current.h>
#include <proc.h>
#include <thread.h>
#include <addrspace.h>
#include <copyinout.h> 
#include <mips/trapframe.h>
  /* this implementation of sys__exit does not do anything with the exit code */
  /* this needs to be fixed to get exit() and waitpid() working properly */

void sys__exit(int exitcode) {

  struct addrspace *as;
  struct proc *p = curproc;
  /* for now, just include this to keep the compiler from complaining about
     an unused variable */
  (void)exitcode;

  DEBUG(DB_SYSCALL,"Syscall: _exit(%d)\n",exitcode);

  KASSERT(curproc->p_addrspace != NULL);
  as_deactivate();
  /*
   * clear p_addrspace before calling as_destroy. Otherwise if
   * as_destroy sleeps (which is quite possible) when we
   * come back we'll be calling as_activate on a
   * half-destroyed address space. This tends to be
   * messily fatal.
   */
  as = curproc_setas(NULL);
  as_destroy(as);

  /* detach this thread from its process */
  /* note: curproc cannot be used after this call */
  proc_remthread(curthread);

  /* if this is the last user process in the system, proc_destroy()
     will wake up the kernel menu thread */
  proc_destroy(p);
  
  thread_exit();
  /* thread_exit() does not return, so we should never get here */
  panic("return from thread_exit in sys_exit\n");
}


/* getpid system call */
int
sys_getpid(pid_t *retval)
{
  *retval = curproc->p_pid;
  return(0);
}

/* stub handler for waitpid() system call                */

int
sys_waitpid(pid_t pid,
	    userptr_t status,
	    int options,
	    pid_t *retval)
{
  int exitstatus;
  int result;

  /* this is just a stub implementation that always reports an
     exit status of 0, regardless of the actual exit status of
     the specified process.   
     In fact, this will return 0 even if the specified process
     is still running, and even if it never existed in the first place.

     Fix this!
  */

  if (options != 0) {
    return(EINVAL);
  }
  /* for now, just pretend the exitstatus is 0 */
  exitstatus = 0;
  result = copyout((void *)&exitstatus,status,sizeof(int));
  if (result) {
    return(result);
  }
  *retval = pid;
  return(0);
}
void uproc_thread(void *temp_tr, unsigned long k);

void uproc_thread(void *temp_tr, unsigned long k) {
  (void) k;
  enter_forked_process(temp_tr);
}

int sys_fork(struct trapframe * tf, pid_t *retval) {
  // Declare temporary trapframe
  struct trapframe * temp_tf;
  // declare addrspace for child
  struct addrspace * child_vmspace = NULL;
  // our child process
  struct proc * child_proc;
  // what thread_fork returns to (for error handling)
  int err;
  DEBUG(DB_SYSCALL, "Syscall: sys_fork()\n");

  // allocate the temporary trapframe. this is critical, so we KASSERT.
  temp_tf = kmalloc(sizeof(struct trapframe));
  if(temp_tf == NULL) {
    panic("Error allocating temporary trapframe in sys_fork\n");
  }
  KASSERT(temp_tf != NULL);
  // copy the address space
  KASSERT(child_vmspace == NULL);
  as_copy(curproc->p_addrspace, &child_vmspace);
  if(child_vmspace == NULL) {
    kprintf("sys_fork: as_copy failed %s\n", strerror(ENOMEM));
    return ENOMEM;
  }
  // call proc_create_fork to create new proc struct for uproc_thread
  child_proc = proc_create_runprogram(curproc->p_name);

  // fill in address space field of proc struct (as created above)
  child_proc->p_addrspace = child_vmspace;

  /* Copy the parent trap frame to temporary trapfram.
   * This is the first of two trapframe compies in the fork transition from 
   * the parent to the child.
   * Note this only does a one level copy, but trap frames don't have pointers
   * except for what we'll fill in from here on.
   */
  *temp_tf = *tf;

  // call thread_fork with proc, uproc_thread, and temp_tf. 
  err = thread_fork(child_proc->p_name, child_proc, uproc_thread, temp_tf, 0);
  if(err) {
    return err;
  }

  // Parent returns to syscall dispatcher (in syscall.c) with the child PID
  *retval = (child_proc->p_pid);
  return (0);
}
