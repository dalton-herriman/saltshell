#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_TOKENS 64
#define DELIMITERS " \t\r\n\a"

void read_line(char *line);
void split_line(char *line, char *tokens[]);
int execute(char *tokens[]);

void shell_loop() {
    char line[1024];  // Buffer to hold the input line
    char *tokens[MAX_TOKENS];  // Array to hold tokens

    while (1) {
        printf("> ");
        read_line(line);           // Read input line
        split_line(line, tokens);  // Split into tokens
        if (execute(tokens) == 0)  // Execute the command
            break;  // Exit shell if command is "exit"
    }
}

void read_line(char *line) {
    fgets(line, sizeof(line), stdin);  // Read a line of input
}

void split_line(char *line, char *tokens[]) {
    int i = 0;
    char *token = strtok(line, DELIMITERS);

    while (token != NULL) {
        tokens[i] = token;  // Store token in the array
        i++;
        token = strtok(NULL, DELIMITERS);  // Get next token
    }
    tokens[i] = NULL;  // Null-terminate the tokens array
}

int execute(char *tokens[]) {
    if (tokens[0] == NULL) {
        return 1;  // Empty command
    }

    if (strcmp(tokens[0], "exit") == 0) {
        return 0;  // Exit the shell
    }

    pid_t pid = fork();  // Create a child process
    if (pid == 0) {
        if (execvp(tokens[0], tokens) == -1) {
            perror("shell");  // Error in exec
        }
        exit(EXIT_FAILURE);  // Exit child process if exec fails
    } else if (pid < 0) {
        perror("shell");  // Error forking
    } else {
        int status;
        waitpid(pid, &status, 0);  // Wait for the child process to finish
    }

    return 1;  // Continue running the shell
}

int main() {
    shell_loop();  // Start the shell loop
    return 0;
}
