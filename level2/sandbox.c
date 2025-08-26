#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
	void	alarm_handler(int sig)
{
	(void)sig;
}
int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	int	pid;
	int	status;

	sigaction(SIGALRM, &(struct sigaction){.sa_handler = alarm_handler}, NULL);
	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		f();
		exit(0);
	}
	alarm(timeout);
	waitpid(pid, &status, 0);
	if (EINTR == errno)
	{
		kill(pid,SIGKILL);
		waitpid(pid, &status, 0);
		if (verbose)
			printf("killed bu alarm lol");
		return 1;
	}
	if (WIFEXITED(status))
	{
		if (WEXITSTATUS(status) == 0)
		{
			if (verbose == true)
				printf("Nice function!\n");
			return (1);
		}
		else
		{
			if (verbose == true)
				printf("Bad function: exited with code %d\n", WEXITSTATUS(status));
			return (0);
		}
	}
	if (WIFSIGNALED(status))
	{
		if (verbose == true)
			printf("Bad function: %s\n", strsignal(WTERMSIG(status)));
		return (0);
	}
	return (-1);
}
