#include "pipex.h"
#include "libft.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

void	error(char *msg, bool in_lib)
{
	if (in_lib)
		perror(msg);
	else
		ft_putstr_fd(msg, STDERR_FILENO);
	exit(EXIT_FAILURE);
}

void	find_path(t_pipex *pipex, char **envp)
{
	char	**env_cpy;

	env_cpy = envp;
	while (*env_cpy && ft_strncmp("PATH=", *env_cpy, 5) != 0)
		++env_cpy;
	if (!*env_cpy)
		error("No PATH environment variable was found", false);
	pipex->path = ft_split(*(env_cpy) + 5, ':');
	if (pipex->path != NULL)
		return ;
	cleanup_fds(pipex);
	error("Allocating PATHs failed for some weird reason", true);
}

bool	open_pipe(int *fds)
{
	int	tmp;

	if (pipe(fds) != 0)
		return (false);
	tmp = fds[0];
	fds[0] = fds[1];
	fds[1] = tmp;
	return (true);
}

void	create_fds(t_pipex *pipex, int argc, char **argv)
{
	int	pipe_index;

	pipex->fd_count = 2 * pipex->cmd_count;
	pipex->fds = ft_calloc(pipex->fd_count, sizeof(int));
	if (!pipex->fds)
		error("Allocating file descriptor array failed", true);
	pipex->fds[0] = open(argv[1], O_RDONLY);
	if (pipex->fds[0] < 0)
		error("Error opening infile", true);
	pipex->fds[pipex->fd_count - 1] = open(
			argv[argc - 1], O_CREAT | O_WRONLY | O_TRUNC, 00644);
	if (pipex->fds[pipex->fd_count - 1] < 0)
		error("Error opening outfile", true);
	pipe_index = 1;
	while (pipe_index < pipex->fd_count - 1)
	{
		if (!open_pipe(pipex->fds + pipe_index))
		{
			cleanup_fds(pipex);
			error("Error opening pipe", true);
		}
		pipe_index += 2;
	}
}

void	create_forks(t_pipex *pipex, char **argv, char **envp)
{
	int		i;
	pid_t	pid;

	i = 0;
	while (i < pipex->cmd_count)
	{
		pid = fork();
		if (pid == -1)
		{
			cleanup_fds(pipex);
			ft_free_mult((void **) pipex->path);
			error("Forking failed!", true);
		}
		else if (pid == 0)
			child(pipex, pipex->fds + i * 2, argv[i + 2], envp);
		++i;
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_pipex	pipex;
	int		status;

	if (argc < 5)
		error("Usage: ./pipex <infile> <cmd1> <cmd2> <cmdn>.. <outfile>", false);
	pipex.cmd_count = argc - 3;
	create_fds(&pipex, argc, argv);
	find_path(&pipex, envp);
	create_forks(&pipex, argv, envp);
	close_pipes(&pipex);
	waitpid(-1, &status, 0);
	cleanup(&pipex, true);
}