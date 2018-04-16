/*
 * Copyright (c) 2013
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _PROC_H_
#define _PROC_H_

/*
 * Definition of a process.
 *
 * Note: curproc is defined by <current.h>.
 */

#include <spinlock.h>
#include <thread.h> /* required for struct threadarray */
#include <array.h>

struct addrspace;
struct vnode;
#ifdef UW
struct semaphore;
#endif // UW

/*
 * Process structure.
 */
struct proc {
	char *p_name;			/* Name of this process */
	struct spinlock p_lock;		/* Lock for this structure */
	struct threadarray p_threads;	/* Threads in this process */
  // removing this until we get proc table working
  // struct array * child_procs; /* children of this process */
	/* VM */
	struct addrspace *p_addrspace;	/* virtual address space */
	/* VFS */
	struct vnode *p_cwd;		/* current working directory */

#ifdef UW
  /* a vnode to refer to the console device */
  /* this is a quick-and-dirty way to get console writes working */
  /* you will probably need to change this when implementing file-related
     system calls, since each process will need to keep track of all files
     it has opened, not just the console. */
  struct vnode *console;                /* a vnode for the console device */
#endif
  pid_t p_pid;
  pid_t p_ppid;
  struct proc * p_pproc;
  // exit, exitcode, and waitpid synchronization stuff
  // need a design for this. syscall specifications requre that a child
  // maintain it's exit status after finishing so long as the parent 
  // may want to do a waitpid
  // Exit codes are easily represented by an int. 0 indicates successful
  // return. Maybe 1 indicates still running? -1 for errors? We'll hash it
  // out as we move forward
  int exitcode;
  // fork, exit, proc_destroy synch stuff
  // we need a design for child-parent synchronization.
  // when can a proc be destroyed? when must it hang around? for how long?
  // who destroys the proc--self or parent?
  // A proc can be destroyed when its parent has called a waitpid and received
  // the exitcode, or if the child process is orphaned
  // since PID_MIN is two, and pid=0 is reserved for the kernel, we can use 
  // 1 to indicate a process' parent has terminated before the child process
  // has completed. 
  // A process must hang around until the parent calls a waitpid, or the 
  // parent process indicates to its children that it has terminated
  // this means whenever a process is destroyed, we have to iterate over
  // the array and change all of its children's parent pids to 1, to indicate
  // that its parent has terminated. The child when finished, but before a 
  // waitpid has been called, will check to see if its parent pid field has 
  // changed to 1, at which point it can clean itself up (and in the process,
  // alerting any of ITS children)
  // the parent destroys the proc, unless its child process has been orphaned
 
  // what about all the fields in the proc? When can they be released?
  // Does the kernel have to make sure parents hang around for all their 
  // children to finish? If not, what happens to the orphaned children?
};

/* This is the process structure for the kernel and for kernel-only threads. */
extern struct proc *kproc;

/* Semaphore used to signal when there are no more processes */
#ifdef UW
extern struct semaphore *no_proc_sem;
#endif // UW

/* Call once during system startup to allocate data structures. */
void proc_bootstrap(void);

/* Create a fresh process for use by runprogram(). */
struct proc *proc_create_runprogram(const char *name);

/* Create a fresh process for use by sysfork, I believe */
struct proc * proc_create_fork(const char * name);

/* Destroy a process. */
void proc_destroy(struct proc *proc);

/* Attach a thread to a process. Must not already have a process. */
int proc_addthread(struct proc *proc, struct thread *t);

/* Detach a thread from its process. */
void proc_remthread(struct thread *t);

/* Fetch the address space of the current process. */
struct addrspace *curproc_getas(void);

/* Change the address space of the current process, and return the old one. */
struct addrspace *curproc_setas(struct addrspace *);


#endif /* _PROC_H_ */
