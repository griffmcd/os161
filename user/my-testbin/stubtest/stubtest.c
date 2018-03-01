#include <unistd.h>
#include <stdio.h>

int main()
{
  pid_t fpid;
  pid_t gpid;
  printf("Test call for fork stub\n");
  fpid = fork();
  printf("pid: %d\n", fpid);
  printf("Test call for getpid stub\n");
  gpid = getpid();
  printf("pid: %d\n", gpid);
  return 0;
}
