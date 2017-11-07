#include <types.h>
#include <lib.h>
#include <test.h>

int usertest1(int nargs, char **args)
{
  (void)nargs;
  (void)args;
  kprintf("This is the user test.\n");
  kprintf("User test complete.\n");
  return 0;
}
