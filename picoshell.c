#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int strlenn(char ***cmds)
{
	int i = 0;
	while (cmds[i])
		i++;
	return i;
}

void dup3(int to,int by)
{
	dup2(to,by);
	close(to);
}

int picoshell(char **cmds[])
{
	int size = strlenn(cmds);
	int curr_pipe[2];
	int prev_pipe = -1;
	int i = 0;
	while(cmds[i])
	{
		if (i != size - 1 && pipe(curr_pipe) == -1)
		{
			if (i != 0)
				close(prev_pipe);
			return 1;
		}
		int id = fork();
		if (id == -1)
		{
			if (i != 0)
				close(prev_pipe);
			close(curr_pipe[0]);
			close(curr_pipe[1]);
			return 1;
		}
		if (id == 0)
		{
			if (i == 0 && size != 1)
			{
				close(curr_pipe[0]);
				dup3(curr_pipe[1],STDOUT_FILENO);
			}
			else if (i != size - 1)
			{
				close(curr_pipe[0]);
				dup3(prev_pipe,STDIN_FILENO);
				dup3(curr_pipe[1],STDOUT_FILENO);
			}
			else if (i == size - 1)
				dup3(prev_pipe,STDIN_FILENO);
			execvp(cmds[i][0],cmds[i]);
			exit(1);	
		}
		if (i != 0)
			close(prev_pipe);
		if (i != size - 1)
		{
			prev_pipe = curr_pipe[0];
			close(curr_pipe[1]);
		}
		i++;
	}
	while (wait(NULL) != -1);
	return 0;
}


int main(int argc, char **argv)
{
	int cmds_size = 1;
	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "|"))
			cmds_size++;
	}
	char ***cmds = calloc(cmds_size + 1, sizeof(char **));
	if (!cmds)
	{
		dprintf(2, "Malloc error: %m\n");
		return 1;
	}
	cmds[0] = argv + 1;
	int cmds_i = 1;
	for (int i = 1; i < argc; i++)
		if (!strcmp(argv[i], "|"))
		{
			cmds[cmds_i] = argv + i + 1;
			argv[i] = NULL;
			cmds_i++;
		}
	int ret = picoshell(cmds);
	if (ret)
		perror("picoshell");
	free(cmds);
	return ret;
}