#include "pipex.h"
#include "libft.h"
#include <stdio.h>
#include <stdlib.h>

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
		error(ERROR_PATH_NF, false);
	pipex->path = ft_split(*(env_cpy) + 5, ':');
	if (pipex->path != NULL)
		return ;
	cleanup(pipex, false);
	error(ERROR_PATH_MAL, true);
}

static void	initialize_pipex(t_pipex *pipex, int argc, char **argv)
{
	if (ft_strncmp(argv[1], "here_doc", 9) != 0)
	{
		pipex->cmd_count = argc - 3;
		pipex->here_doc = false;
	}
	else
	{
		pipex->cmd_count = argc - 4;
		pipex->here_doc = true;
	}
	if (pipex->cmd_count < 2)
		error(ERROR_COMMANDS, false);
	pipex->children = ft_calloc(pipex->cmd_count, sizeof(pid_t));
	if (!pipex->children)
		error(ERROR_PID_MAL, true);
	pipex->fd_count = 2 * pipex->cmd_count;
	pipex->fds = ft_calloc(pipex->fd_count, sizeof(int));
	if (pipex->fds)
		return ;
	free(pipex->children);
	error(ERROR_FD_MAL, true);
}

int	main(int argc, char **argv, char **envp)
{
	t_pipex	pipex;
	int		status;

	if (argc < 5)
		error(ERROR_USAGE, false);
	initialize_pipex(&pipex, argc, argv);
	find_path(&pipex, envp);
	open_files(&pipex, argc, argv);
	open_pipes(&pipex);
	create_forks(&pipex, argv, envp);
	close_pipes(&pipex);
	status = wait_for_children(&pipex);
	cleanup(&pipex, true);
	cleanup_heredoc();
	exit(status);
}
