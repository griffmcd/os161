/*
 * forkchain - test fork().
 *
 * This should work correctly when fork is implemented.
 *
 * It should also continue to work after subsequent assignments, most
 * notably after implementing the virtual memory system.
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

/*
 * This is used by all processes, to try to help make sure all
 * processes have a distinct address space.
 */
static volatile int mypid;
static volatile int limit;

/*
 * Helper function for fork that prints a warning on error.
 */
static
int
dofork(int c)
{
  if(c >= limit) {
    return 0;
  }
	int pid;
	pid = fork();
  dofork(++c);
	if (pid < 0) {
		warn("fork");
	}
  printf("%d", pid);
	return pid;
}

static
void
test(int c)
{
  int pid;
	pid = dofork(c);
	putchar('\n');
}

int
main(int argc, char *argv[])
{

  if(argc==1) {
    limit = atoi(argv[1]);
  }else {
    limit = 10;
  }
	warnx("Starting.");

	test(0);

	warnx("Complete.");
	return 0;
}
