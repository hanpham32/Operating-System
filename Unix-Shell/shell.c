#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MY_MAX_LINE 80 /* maximum length command */

int main(void)
{
	char *args[MY_MAX_LINE / 2 + 1]; /* command line arguments */
	int should_run = 1;				 /* flag to determine when to exit program */
	char archived_command[80] = "";

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
		int concurrently = 0;
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
			pid = fork();
			if (pid == 0) // Child process
			{
				// Execute
				if (execvp(args[0], args) == -1)
				{
					perror("osh");
				}
				exit(EXIT_FAILURE); // returns on error
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

		/**
		 * After reading user input, the steps are:
		 * (1) fork a child process using fork()
		 * (2) the child process will invoke execvp()
		 * (3) parent will invoke wait() unless command included &
		 */
	}
	return 0;
}
