  #include <sys/types.h>
  #include <stdio.h>
  #include <unistd.h>  
  #include <sys/wait.h>
  #define SIZE 5
  int nums[SIZE] = {0,1,2,3,4};
  int main()
  {
     int i;
     pid_t pid;
  
     pid = fork();
     if (pid == 0) {
        for (i = 0; i < SIZE; i++) {
        nums[i] *= -i;
        printf("CHILD: %d ",nums[i]); /* LINE X */
        }
     }
     else if (pid > 0) {
        // waitpid(pid, NULL, 0);
	int status; // Integer to store the exit status
        wait(&status); // Pass the address of the status variable
        for (i = 0; i < SIZE; i++)
           printf("PARENT: %d ",nums[i]); /* LINE Y */
     }
     return 0;
  }

