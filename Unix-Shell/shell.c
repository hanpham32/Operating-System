#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MY_MAX_LINE 80 /* maximum length command */

/* Function to handle ">" operator */
void handle_output_redirection(char *filename)
{
	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("osh");
		exit(EXIT_FAILURE);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
}

/* Function to handle "<" operator */
void handle_input_redirection(char *filename)
{
	int fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		perror("osh");
		exit(EXIT_FAILURE);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
}

/* Function to process pipe*/
void execute_pipe(char *command1[], char *command2[])
{
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		perror("osh");
		exit(EXIT_FAILURE);
	}

	pid_t pid1, pid2;
	if ((pid1 = fork()) == 0) // First child process
	{
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		if (execvp(command1[0], command1) == -1)
		{
			perror("osh");
			exit(EXIT_FAILURE);
		}
	}

	if ((pid2 = fork()) == 0) // Second child process
	{
		close(pipefd[1]);
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);
		if (execvp(command2[0], command2) == -1)
		{
			perror("osh");
			exit(EXIT_FAILURE);
		}
	}

	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
}

int main(void)
{
	char *args[MY_MAX_LINE / 2 + 1]; /* command line arguments */
	int should_run = 1;				 /* flag to determine when to exit program */
	char archived_command[80] = "";	 /* store previous user command */

	while (should_run)
	{
		char str[80];
		printf("osh>");
		fflush(stdout); /* ensures osh> is displayed before waiting for user input */

		// READ USER INPUT
		fgets(str, MY_MAX_LINE, stdin);
		char *newline = strchr(str, '\n'); // find the line character
		if (newline)
		{
			*newline = '\0'; // replace the newline character with a null terminator
		}

		// Check for "!!" command; History feature
		if (strcmp(str, "!!") == 0)
		{
			if (strlen(archived_command) == 0)
			{
				printf("No commands in history. \n");
				continue;
			}
			else
			{
				printf("%s\n", archived_command);
				strncpy(str, archived_command, sizeof(str));
			}
		}
		else
		{
			strncpy(archived_command, str, sizeof(archived_command));
		}

		// Split string
		char *token;
		int i = 0;
		token = strtok(str, " ");
		while (token != NULL)
		{
			args[i] = token;
			token = strtok(NULL, " ");
			i++;
		}
		args[i] = NULL;

		// check for ampersand
		int concurrently = 0; /* flag for ampersand */
		if (i > 0 && strcmp(args[i - 1], "&") == 0)
		{
			concurrently = 1;
			args[i - 1] = NULL;
		}

		// EXIT
		if (strcmp(str, "exit") == 0)
		{
			printf("shell terminated \n");
			should_run = 0;
		}
		else
		{
			pid_t pid, wpid;
			int status = 0;
			char *command1[MY_MAX_LINE / 2 + 1];
			char *command2[MY_MAX_LINE / 2 + 1];

			// Check for pipe
			int is_pipe = 0;
			for (int i = 0; args[i] != NULL; i++)
			{
				if (strcmp(args[i], "|") == 0)
				{
					is_pipe = 1; // flag for pipe
					args[i] = NULL;
					// parse the first command
					for (int j = 0; args[i + j + 1] != NULL; j++)
					{
						command2[j] = args[i + j + 1];
						command2[j + 1] = NULL;
					}
					break;
				}
				command1[i] = args[i];
				command1[i + 1] = NULL;
			}

			pid = fork();

			if (pid == 0) // Child process
			{
				// Execute
				if (is_pipe)
				{
					execute_pipe(command1, command2);
					exit(EXIT_SUCCESS);
				}
				else
				{
					for (int j = 0; args[j] != NULL; j++)
					{
						if (strcmp(args[j], ">") == 0)
						{
							args[j] = NULL;
							handle_output_redirection(args[j + 1]);
						}
						else if (strcmp(args[j], "<") == 0)
						{
							args[j] = NULL;
							handle_input_redirection(args[j + 1]);
						}
					}
					if (execvp(args[0], args) == -1)
					{
						perror("osh");
					}
					exit(EXIT_FAILURE); // returns on error
				}
			}
			else if (pid < 0) // Forking error
			{
				perror("osh");
			}
			else
			{
				// Parent process
				if (!concurrently) // invoke wait because `&` is included
				{
					do
					{
						wpid = waitpid(pid, &status, WUNTRACED);
					} while (!WIFEXITED(status) && !WIFSIGNALED(status));
				}
			}
		}
	}
	return 0;
}
