#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

//Allowed functions:	close, fork, wait, exit, execvp, dup2, pipe


int    picoshell(char **cmds[])
{
	int pipes[2];
	int prev_pipe = -1;
	int i = 0;
	int pid;
	int status;
	while(cmds[i])
	{
		if(cmds[i + 1])
		{
			if (pipe(pipes) == -1)
			{
				if (prev_pipe != -1)
				close(prev_pipe);
				return 1;
			}
		}
		pid = fork();
		if (pid == -1)
		{
			if (i != 0)
				close(prev_pipe);
			if (cmds[i + 1])
			{
				close(pipes[0]);
				close(pipes[1]);
			}
			return 1;
		}
		if (pid == 0)
		{
			if (i == 0)
			{
				dup2(STDOUT_FILENO,pipes[1]);
				close(pipes[1]);
				close(pipes[0]);
				execvp(cmds[i][0], cmds[i]);
				exit(1);
			}
			else if (i > 0 && cmds[i + 1])
			{
				dup2(STDOUT_FILENO,pipes[1]);
				dup2(STDIN_FILENO,prev_pipe);
				close(prev_pipe);
				close(pipes[1]);
				close(pipes[0]);
				execvp(cmds[i][0], cmds[i]);
				exit(1);
			}
			else if (!cmds[i + 1])
			{
				dup2(STDIN_FILENO,prev_pipe);
				close(prev_pipe);
				execvp(cmds[i][0], cmds[i]);
				exit(1);
			}
		}
		if (i == 0)
		{
			close(pipes[1]);
			prev_pipe = pipes[0];
		}
		else if (i > 0 && cmds[i + 1])
		{
			close(prev_pipe);
			close(pipes[1]);
			prev_pipe = pipes[0];
		}
		else if (!cmds[ i + 1])
			close(prev_pipe);
		i++;
	}
	while(wait(NULL));
	return 0;
}
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
static int	count_cmds(int argc, char **argv)
{
	int	count = 1;
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "|") == 0)
			count++;
	}
	return (count);
}

int	main(int argc, char **argv)
{
	if (argc < 2)
		return (fprintf(stderr, "Usage: %s cmd1 [args] | cmd2 [args] ...\n", argv[0]), 1);

	int	cmd_count = count_cmds(argc, argv);
	char	***cmds = calloc(cmd_count + 1, sizeof(char **));
	if (!cmds)
		return (perror("calloc"), 1);

	int	i = 1, j = 0;
	while (i < argc)
	{
		int	len = 0;
		while (i + len < argc && strcmp(argv[i + len], "|") != 0)
			len++;
		cmds[j] = calloc(len + 1, sizeof(char *));
		if (!cmds[j])
			return (perror("calloc"), 1);
		for (int k = 0; k < len; k++)
			cmds[j][k] = argv[i + k];
		cmds[j][len] = NULL;
		i += len + 1;
		j++;
	}
	cmds[cmd_count] = NULL;

	int	ret = picoshell(cmds);

	// Clean up
	for (int i = 0; cmds[i]; i++)
		free(cmds[i]);
	free(cmds);

	return (ret);
}