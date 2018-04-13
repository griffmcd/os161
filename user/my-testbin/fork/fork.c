#include <unistd.h>
#include <stdio.h>

int main()
{
  pid_t pid;
  printf("Test call for fork stub\n");
  pid = fork();
  printf("pid: %d\n", pid);
  return 0; // avoid compiler warnings
}
