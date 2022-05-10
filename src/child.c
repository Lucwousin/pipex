/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   child.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: lsinke <lsinke@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/10 12:22:16 by lsinke        #+#    #+#                 */
/*   Updated: 2022/05/10 12:22:16 by lsinke        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include "libft.h"

static char	*find_executable(char *cmd, char **path)
{
	char	*command;

	cmd = ft_strjoin("/", cmd);
	if (!cmd)
		error(ERROR_CMD_MAL, true);
	command = NULL;
	while (*path)
	{
		command = ft_strjoin(*path, cmd);
		if (access(command, X_OK) == 0)
			break ;
		free(command);
		command = NULL;
		++path;
	}
	free(cmd);
	return (command);
}

void	child(t_pipex *pipex, int *fds, char *cmd, char **envp)
{
	char	*cmd_path;
	char	**argv;
	bool	dup_failed;

	dup_failed = false;
	dup_failed |= dup2(fds[0], STDIN_FILENO) < 0;
	dup_failed |= dup2(fds[1], STDOUT_FILENO) < 0;
	close_pipes(pipex);
	if (dup_failed)
	{
		cleanup(pipex, false);
		error(ERROR_DUP2, true);
	}
	cmd_path = NULL;
	argv = ft_split(cmd, ' ');
	if (argv)
		cmd_path = find_executable(argv[0], pipex->path);
	if (cmd_path)
		execve(cmd_path, argv, envp);
	cleanup_child(pipex, argv);
	if (!cmd_path)
		error(ERROR_CMD_NF, false);
	free(cmd_path);
	error(ERROR_EXEC, true);
}
