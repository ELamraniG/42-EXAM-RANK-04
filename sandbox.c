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
				printf("Bad function: exited with code %d\n",
					WEXITSTATUS(status));
			return (0);
		}
	}
	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGALRM)
		{
			if (verbose == true)
				printf("killed by time %s\n", strsignal(WTERMSIG(status)));
		}
		else
		{
			if (verbose == true)
				printf("Bad function: %s\n", strsignal(WTERMSIG(status)));
		}
		return (0);
	}
	return (-1);
}

void	nice_function(void)
{
	// This function does nothing and exits normally (exit code 0)
	return ;
}

void	bad_function_exit_code(void)
{
	// This function exits with code 1 (failure)
	exit(1);
}

void	bad_function_segfault(void)
{
	int	*ptr;

	// This function causes a segmentation fault
	ptr = NULL;
	*ptr = 42; // This will cause a segfault
}

void	bad_function_timeout(void)
{
	// This function runs indefinitely
	while (1)
	{
	}
}

void	bad_function_sleep(void)
{
	// This function sleeps for more than the timeout and gets killed by alarm handler
	sleep(5); // Sleep for 5 seconds (assuming timeout is less than 5 seconds)
}

int	main(void)
{
	int	result;

	printf("Test 1: Normal function (Nice)\n");
	result = sandbox(nice_function, 5, true);
	printf("Result: %d\n", result); // Expected output: 1 ("Nice function!")
	printf("Test 2: Bad function (Exit code 1)\n");
	result = sandbox(bad_function_exit_code, 5, true);
	printf("Result: %d\n", result);
		// Expected output: 0 ("Bad function: exited with code 1")
	printf("Test 3: Bad function (Segfault)\n");
	result = sandbox(bad_function_segfault, 5, true);
	printf("Result: %d\n", result);
		// Expected output: 0 ("Bad function: Segmentation fault")
	printf("Test 4: Bad function (Timeout)\n");
	result = sandbox(bad_function_timeout, 2, true); // Timeout after 2 seconds
	printf("Result: %d\n", result);                 
		// Expected output: 0 ("Bad function: timed out after 2 seconds")
	printf("Test 5: Bad function (Killed by SIGKILL)\n");
	result = sandbox(bad_function_sleep, 2, true); // Timeout after 2 seconds
	printf("Result: %d\n", result);               
		// Expected output: 0 ("Bad function: timed out after 2 seconds")
	return (0);
}
