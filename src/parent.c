/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parent.c                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: lsinke <lsinke@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/10 12:22:11 by lsinke        #+#    #+#                 */
/*   Updated: 2022/05/10 12:22:11 by lsinke        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include <sys/wait.h>

/**
 * Fork for all the sub-processes we need
 *
 * Save the PID in an array so we can keep track of the last one's return value
 */
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
			cleanup(pipex, true);
			error(ERROR_FORK, true);
		}
		else if (pid == 0)
			child(pipex, pipex->fds + i * 2, argv[i + cmd_offset], envp);
		else if (i == pipex->cmd_count - 1)
			pipex->last_cmd_pid = pid;
		++i;
	}
}

/**
 * Wait for all subprocesses. Return the exit code of the last command
 */
int	wait_for_children(t_pipex *pipex)
{
	int		status;
	int		last_command_status;
	pid_t	pid;

	last_command_status = -1;
	while (true)
	{
		pid = wait(&status);
		if (pid < 0)
			break ;
		if (pid == pipex->last_cmd_pid)
			last_command_status = status;
	}
	if (WIFEXITED(last_command_status))
		return (WEXITSTATUS(last_command_status));
	return (0);
}
