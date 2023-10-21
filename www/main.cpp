#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void handle_alarm(int signum)
{
    printf("Child: Alarm went off!\n");
    exit(1);
}

int main()
{
    pid_t child_pid = fork();

    if (child_pid == 0)
    {
        // This is the child process
        // signal(SIGALRM, handle_alarm);
        alarm(1);

        // Simulate some work in the child process
        sleep(5);

        // The child process does not explicitly call exit,
        // so its exit status will be determined by the return value
        // from this code block.
        printf("Child: Exiting\n");
    }
    else if (child_pid > 0)
    {
        // This is the parent process
        int status;
        wait(&status);

        if (WIFEXITED(status))
        {
            printf("Parent: Child exited with status %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("Parent: Child exited with status %d   %d    %d\n", WTERMSIG(status) , WCOREDUMP(status), SIGALRM);
        }
    }
    else
    {
        perror("fork failed");
        return 1;
    }

    return 0;
}
