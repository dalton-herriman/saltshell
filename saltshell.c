#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


char *saltshell_readline(void) {
    char *line = NULL;
    size_t bufsize = 0; // Let getline allocate
    getline(&line, &bufsize, stdin);
    return line;
}

#define SALTSHELL_TOK_BUFSIZE 64
#define SALTSHELL_TOK_DELIM " \t\r\n\a"

char **saltshell_splitline(char *line) {
    int bufsize = SALTSHELL_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "saltshell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SALTSHELL_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += SALTSHELL_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "saltshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, SALTSHELL_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int saltshell_execute(char **args) {
    if (args[0] == NULL) {
        // Empty command
        return 1;
    }

    if (strcmp(args[0], "exit") == 0) {
        return 0;  // Exit the shell
    }

    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("saltshell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("saltshell");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1; // Keep shell running
}

void saltshell_loop(void) {
	char *line;
	char **args;
	int status;

	do {
		printf("> ");
		line = saltshell_readline();
		args = saltshell_splitline(line);
		status = saltshell_execute(args);

		free(line);
		free(args);
	} while (status); 
}


int main(int argc, char **argv) {
	// Load config files

	// Run command loop
	saltshell_loop();

	// Perform shutdown/cleanup
	return 0;
}