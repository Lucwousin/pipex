#include "pipex.h"
#include "libft.h"

static char	*find_executable(char *cmd, char **path)
{
	char	*command;

	cmd = ft_strjoin("/", cmd);
	if (!cmd)
		error("Allocating command failed", true);
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

	if (dup2(fds[0], STDIN_FILENO) < 0)
		error("Duplicating file descriptor failed", true);
	if (dup2(fds[1], STDOUT_FILENO) < 0)
		error("Duplicating file descriptor failed", true);
	close_pipes(pipex);
	argv = ft_split(cmd, ' ');
	cmd_path = find_executable(argv[0], pipex->path);
	if (cmd_path)
		execve(cmd_path, argv, envp);
	cleanup_child(pipex, argv);
	if (!cmd_path)
		error("Command not found!", false);
	free(cmd_path);
	error("Something went wrong during execve call", true);
}