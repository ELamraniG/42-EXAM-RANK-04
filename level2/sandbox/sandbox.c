 #include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
 #include <string.h>
 #include <stdio.h>
  #include <stdlib.h>
void alarm_handler(int sig)
{
    (void)sig;
}

int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
    int pid;
    int status;
    pid = fork();
    if (pid == -1)
        return -1;
    sigaction(SIGALRM, &(struct  sigaction){.sa_handler=alarm_handler},NULL);
    if (pid == 0)
    {
        f();
        exit(0);
    }
    alarm(timeout);
    waitpid(pid,&status,0);
    if (errno == EINTR)
    {

        kill(pid,SIGKILL);
        waitpid(pid,&status,0);
        if (verbose)
            printf("Bad function: timed out after %d seconds\n",timeout);
        return 0;
    }
    else if (WIFEXITED(status))
    {
        if(WEXITSTATUS(status) == 0)
        {

            if (verbose)
                printf("Nice function!\n");
            return 1;
        }
        else
        {
            if (verbose)
                printf("Bad function: exited with code %d\n",WEXITSTATUS(status));
            return 0;
        }
    }
    else if (WIFSIGNALED(status))
    {
        if (verbose)
            printf("Bad function: %s\n",strsignal(WTERMSIG(status)));
        return 0;
    }
    return -1;
}
