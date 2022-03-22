#include <stdio.h>

extern char** environ;

int main(int argc, char** argv, char** envp) {
  printf("Hello world!\n");

  printf("There are %d args, argv = %lx, args are:\n", argc, argv);
  for (int i = 0; i < argc; ++i) {
      printf("arg[%d] = %s\n", i, argv[i]);
  }
  printf("\n");

  printf("envp = %lx, environment variables are:\n", envp);
  for (int i = 0; envp[i]; ++i) {
      printf("envp[%d] = %s\n", i, envp[i]);
  }
  printf("\n");

  printf("environ = %lx, environment variables are:\n", environ);
  for (int i = 0; environ[i]; ++i) {
      printf("environ[%d] = %s\n", i, environ[i]);
  }

  return 0;
}
