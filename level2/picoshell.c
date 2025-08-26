#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
int    picoshell(char **cmds[])
{
	int i = 0;
	int fd[2];
	int prev_pipe = -1;
	int pid;
	while (cmds[i])
	{
		if (cmds[i + 1])
		{
			if (pipe(fd) == -1)
			{
				if(prev_pipe != -1)
					close(prev_pipe);
				return 1;
			}
		}
		pid = fork();
		if (pid == -1)
		{
			if (prev_pipe != -1)
				close(prev_pipe);
			if(cmds[i + 1])
			{
				close(fd[0]);
				close(fd[1]);
			}
			return 1;
		}
		if (pid == 0)
		{
			if(cmds[i + 1])
				dup2(fd[1],STDOUT_FILENO);
			if (prev_pipe != -1)
				dup2(prev_pipe,STDIN_FILENO);
			if (prev_pipe != -1)
				close(prev_pipe);
			if (cmds[i + 1])
			{
				close(fd[1]);
				close(fd[0]);
			}
			execvp(cmds[i][0],cmds[i]);
			exit(1);
		}
		if (prev_pipe != -1)
			close(prev_pipe);
		if (cmds[i + 1])
		{
			prev_pipe = fd[0];
			close(fd[1]);
		}
		i++;
	}
	while (wait(NULL) > 0);
	return 0;
}