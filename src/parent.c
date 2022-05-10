#include "pipex.h"
#include <sys/wait.h>

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
		else
			pipex->children[i] = pid;
		++i;
	}
}

int	wait_for_children(t_pipex *pipex)
{
	int		status;
	int		last_command_status;
	pid_t	pid;

	while (true)
	{
		pid = wait(&status);
		if (pid < 0)
			break ;
		if (pid == pipex->children[pipex->cmd_count - 1])
			last_command_status = status;
	}
	return (last_command_status);
}
