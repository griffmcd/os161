#include <unistd.h>
#include <stdio.h>
#include <time.h>
#define SEC_IN_YEAR 31556926

int main()
{
  printf("Hello World!\n");
  time_t t = time(0);
  printf("current time: %ld\n", (long int) t);
  // printf("Current time: %s\n", ctime(&t));
  return 0;
}
