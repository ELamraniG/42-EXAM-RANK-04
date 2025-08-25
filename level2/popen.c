#include <unistd.h>
#include <stdio.h>
int	ft_popen(const char *file, char *const argv[], char type)
{
	int	pipes[2];
	int	id;

	if ((type != 'r' && type != 'w') || !file || !argv)
		return (-1);
	if (pipe(pipes) == -1)
		return (-1);
	if (type == 'r')
	{
		id = fork();
		if (id == -1)
		{
			close(pipes[1]);
			close(pipes[0]);
			return (-1);
		}
		if (id == 0)
		{
			dup2(pipes[1], STDOUT_FILENO);
			close(pipes[0]);
			close(pipes[1]);
			execvp(file, argv);
			exit(-1);
		}
		close(pipes[1]);
		return (pipes[0]);
	}
	if (type == 'w')
	{
		id = fork();
		if (id == -1)
		{
			close(pipes[1]);
			close(pipes[0]);
			return (-1);
		}
		if (id == 0)
		{
			dup2(pipes[0], STDIN_FILENO);
			close(pipes[0]);
			close(pipes[1]);
			execvp(file, argv);
			exit(-1);
		}
		close(pipes[0]);
		return (pipes[1]);
	}
	return (-1);
}

int	main() {
	int	fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
	dup2(fd, 0);
	fd = ft_popen("grep", (char *const []){"grep", "c", NULL}, 'r');
	char	*line;
	while ((line = get_next_line(fd)))
		printf("%s", line);
}