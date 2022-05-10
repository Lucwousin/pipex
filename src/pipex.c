/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   pipex.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: lsinke <lsinke@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/10 12:22:03 by lsinke        #+#    #+#                 */
/*   Updated: 2022/05/10 12:22:03 by lsinke        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include "libft.h"
#include <stdio.h>
#include <stdlib.h>

void	error_exit_code(char *msg, bool in_lib, int exit_code)
{
	if (in_lib)
		perror(msg);
	else
		ft_putendl_fd(msg, STDERR_FILENO);
	exit(exit_code);
}

/**
 * Print a message to stderr, with an error message if in_lib is true
 * Exit with EXIT_FAILURE
 */
void	error(char *msg, bool in_lib)
{
	error_exit_code(msg, in_lib, EXIT_FAILURE);
}

/**
 * Initialize our program
 *
 * Finds the command count, and allocates 2 arrays
 * One array is going to be for the childrens' PIDs
 * The other is for all file descriptors we need
 */
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

/**
 * Find the PATH environment variable, and split it up into it's parts
 *
 * Because we initialized the FD and PID arrays already, free those if we error
 */
static void	find_path(t_pipex *pipex, char **envp)
{
	char	**env_cpy;

	env_cpy = envp;
	while (*env_cpy && ft_strncmp("PATH=", *env_cpy, 5) != 0)
		++env_cpy;
	if (!*env_cpy)
	{
		cleanup(pipex, false);
		error(ERROR_PATH_NF, false);
	}
	pipex->path = ft_split(*(env_cpy) + 5, ':');
	if (pipex->path != NULL)
		return ;
	cleanup(pipex, false);
	error(ERROR_PATH_MAL, true);
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
	exit(status);
}
