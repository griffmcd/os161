#include <stdio.h>

int say_hello_fn(char *name) {
  printf("Hello %s\n", name);
  return 0;
}

int say_goodbye_fn(char *name) {
	printf("Goodbye %s\n", name);
	return 0;
}

int print_menu_fn() {
	printf("Enter an integer to select a function to run!\n");
	printf("Enter 0 to reprint this menu.\n");
	printf("Currently enabled test functions:\n");
	printf("1 - hello\n");
	printf("2 - goodbye (will exit as well)\n");
	printf("\n");
  return 0;
}
int (*funcs[])()= {*print_menu_fn,
							     *say_hello_fn, 
						       *say_goodbye_fn};

int main(int argc, char * argv[]) {
	char* name = argv[1];
  int user_input;

  funcs[1](name); // say_hello_fn
  while(1) {
    funcs[0]();     // print_menu_fn
    scanf("%d", &user_input);
    if(user_input < 0 || user_input > sizeof(funcs)) {
      printf("User did not enter a valid integer.\n");
    }else if(user_input == 2) {
      break; // user has decided to quit
    }else {
      funcs[user_input]();
    }
  }
	funcs[2](name); // say_goodbye_fn
	exit(0);
}
