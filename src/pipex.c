#include "pipex.h"
#include "libft.h"
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

void	create_forks(t_pipex *pipex, char **argv, char **envp)
{
	int		i;
	int		cmd_offset;
	pid_t	pid;

	if (pipex->here_doc)
		cmd_offset = 3;
	else
		cmd_offset = 2;
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
			child(pipex, pipex->fds + i * 2, argv[i + cmd_offset], envp);
		++i;
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_pipex	pipex;
	int		status;

	if (argc < 5)
		error("Usage: ./pipex <infile> <cmd1> <cmd2> <cmdn>.. <outfile>", false);
	args_in(&pipex, argc, argv);
	create_fds(&pipex, argc, argv);
	find_path(&pipex, envp);
	create_forks(&pipex, argv, envp);
	close_pipes(&pipex);
	waitpid(-1, &status, 0);
	cleanup(&pipex, true);
	cleanup_heredoc();
}