/*
 * forkfan - test fork().
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

static volatile int limit;

static
void
test(void)
{
  int pid = -1;
  for(int i = 0; i < limit; ++i) {
    pid = fork();
  }
  printf("%d\n", pid);
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

	test();

	warnx("Complete.");
	return 0;
}
