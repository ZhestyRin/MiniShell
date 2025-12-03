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
   while(args[i] != NULL && i < MAX_ARGS - 1) {
       i++;
       args[i] = strtok(NULL, " ");
   }
   args[i] = NULL;
}

void handle_redirection(char **args) {
    int i = 0;
    while (args[i] != NULL) {
        if (strcmp(args[i], ">") == 0) {
            if (args[i+1] != NULL) {
                int fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
                args[i] = NULL;
                args[i+1] = NULL; 
            } else {
                fprintf(stderr, "Shell error: no output file specified for >\n");
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(args[i], "<") == 0) {
            if (args[i+1] != NULL) {
                int fd = open(args[i+1], O_RDONLY);
                if (fd == -1) {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
                args[i] = NULL;
                args[i+1] = NULL;
            } else {
                fprintf(stderr, "Shell error: no input file specified for <\n");
                exit(EXIT_FAILURE);
            }
        }
        i++;
    }
}

void handle_cd(char **args) {
    if (args[1] == NULL) {
        char *home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "cd: HOME not set\n");
        } else {
            if (chdir(home) != 0) {
                perror("chdir");
            }
        }
    } else {
        if (chdir(args[1]) != 0) {
            perror("chdir");
        }
    }
}

int main() {
    char input[LSH_BF];
    char *args[MAX_ARGS];

    while (1) {
        printf("Shellulose>> ");

        if(fgets(input, sizeof(input), stdin) == 0) {
            printf("\nExiting shell\n");
            break;
        }

        input[strcspn(input, "\n")] = 0;

        parse(args, input);

        if (args[0] == NULL) {
            continue;
        }

        
        if (strcmp(args[0], "exit") == 0) {
            printf("Exiting shell\n");
            break;
        }
        if (strcmp(args[0], "cd") == 0) {
            handle_cd(args);
        } else {
            
            pid_t pid = fork();

            if(pid == 0) { 
                handle_redirection(args);

                if(execvp(args[0], args) == -1) {
                    perror("Shell error");
                }
                exit(EXIT_FAILURE);
            } else if(pid > 0) {
                wait(NULL);
            } else {
                perror("fork");
            }
        }
    }

    return 0;
}
