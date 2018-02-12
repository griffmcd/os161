/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
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

/*
 * Thread test code.
 */
#include <types.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>
#include <test.h>

#define NTHREADS 10 
static volatile int COUNT=0;

static struct semaphore *tsem = NULL;

static
void
init_sem(void)
{
	if (tsem==NULL) {
		tsem = sem_create("tsem", 0);
		if (tsem == NULL) {
			panic("threadtest: sem_create failed\n");
		}
	}
}

static void destroy_all(void) {
  if(tsem != NULL) { 
    sem_destroy(tsem);
    tsem = NULL;
  }
}

static void unsafethread(void *junk,unsigned long num) {
  (void)junk;
  int i;
  int limit = (int) num;
  for(i=0; i < limit; ++i) {
    COUNT++;
  }
  V(tsem);
}

static void unsafethreadcounter(int threads, int inc) {
  char name[16];
  int i, result;
  for(i=0; i<threads; ++i) {
    snprintf(name, sizeof(name), "unsafe counter test %d", i);
    result = thread_fork(name, NULL, unsafethread, NULL, inc);
    if(result) {
      panic("unsafe thread counter test: thread_fork failed %s)\n",
          strerror(result));
    }
  }
  for(i=0; i < threads; ++i) {
    P(tsem);
  }
}

int
counterfun(int nargs, char ** args)
{
	//(void)nargs;
	// (void)args;
  int n_threads = atoi(args[1]);
  int n_inc;
  if(nargs < 3) {
    n_inc = 1;
  }else {
    n_inc = atoi(args[2]);
  }
	init_sem();
  kprintf("count is %d\n", COUNT);
	kprintf("Starting counter test...\n");
	unsafethreadcounter(n_threads, n_inc);
	kprintf("\ncounter test done.\n");
  destroy_all();
  kprintf("count should be %d\n", (n_threads * n_inc));
  kprintf("count is %d\n", COUNT);

	return 0;
}
