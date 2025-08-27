
#include <unistd.h>
#include <stdlib.h>
//Allowed functions: pipe, fork, dup2, execvp, close, exit
int ft_popen(const char *file, char *const argv[], char type)
{
	int fd[2];
	if (type != 'r' && type != 'w')
		return -1;
	if (pipe(fd) == -1)
		return -1;
	int pid = fork();
	if (pid == -1)
	{
		close(fd[0]);
		close(fd[1]);
		return -1;
	}
	if (pid == 0)
	{
		if (type == 'r')
			dup2(fd[1], STDOUT_FILENO);
		if (type == 'w')
			dup2(fd[0], STDIN_FILENO);
		close(fd[1]);
		close(fd[0]);
		execvp(file,argv);
		exit(-1);
	}
	if (type == 'r')
	{
		close(fd[1]);
		return fd[0];
	}
	if (type == 'w')
	{
		close(fd[0]);
		return fd[1];
	}
	return (-1);
}