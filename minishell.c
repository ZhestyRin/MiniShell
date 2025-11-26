#include<stdio.h>
#include<sys/wait.h>
#include<errno.h>
#include<sys/types.h>
#include<dirent.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#define LSH_BF 1024
#define MAX_ARGS 64



void parse(char **args, char *input) {

  int i = 0;
  args[i] = strtok(input, " ");
    
   while(args[i] != NULL && i < LSH_BF - 1) {
   i++;
   args[i] = strtok(NULL, " ");

  }


}

int main() {

char input[LSH_BF];
char *args[MAX_ARGS];


while (1) {

    printf("RinShell>> ");
 
    if(fgets(input, sizeof(input), stdin) == 0) {
      break;
    }
      
  
   if(strncmp(input, "exit", 4) == 0) {
       printf("Exiting shell\n");
      break;

    }
    input[strcspn(input, "\n")] = 0;


pid_t pid = fork();

 parse(args, input);

   if(pid == 0) {

      if(execvp(args[0], args) == -1) {
       perror("Shell error\n");
    }
     exit(EXIT_FAILURE);
    }
   else if(pid > 0) {
     wait(NULL);

    }
   else {
      perror("fork");
    }

}

  return 0;
}


