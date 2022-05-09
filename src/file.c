#include "pipex.h"
#include "libft.h"
#include "get_next_line.h"
#include <fcntl.h>

static bool	open_pipe(int *fds)
{
	int	tmp;

	if (pipe(fds) != 0)
		return (false);
	tmp = fds[0];
	fds[0] = fds[1];
	fds[1] = tmp;
	return (true);
}

static void	open_files(t_pipex *pipex, int argc, char **argv)
{
	int	out_flags;

	if (!pipex->here_doc)
	{
		out_flags =  O_CREAT | O_WRONLY | O_TRUNC;
		pipex->fds[0] = open(argv[1], O_RDONLY);
		if (pipex->fds[0] < 0)
			error("Error opening infile", true);
	}
	else
	{
		out_flags = O_CREAT | O_WRONLY | O_APPEND;
		pipex->fds[0] = open(HERE_DOC, O_RDONLY);
		if (pipex->fds[0] < 0)
		{
			cleanup_heredoc();
			error("Error opening infile", true);
		}
	}
	pipex->fds[pipex->fd_count - 1] = open(argv[argc - 1], out_flags, 00644);
	if (pipex->fds[pipex->fd_count - 1] < 0)
		error("Error opening outfile", true);
}

void	create_fds(t_pipex *pipex, int argc, char **argv)
{
	int	pipe_index;

	pipex->fd_count = 2 * pipex->cmd_count;
	pipex->fds = ft_calloc(pipex->fd_count, sizeof(int));
	if (!pipex->fds)
		error("Allocating file descriptor array failed", true);
	open_files(pipex, argc, argv);
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

static void	read_stdin(const char *delimiter)
{
	int		fd;
	char	*line;

	fd = open(HERE_DOC, O_CREAT | O_WRONLY | O_TRUNC, 00644);
	if (fd < 0)
		error("Failed to open/create here_doc", true);
	while (true)
	{
		write(STDOUT_FILENO, "> ", 2);
		line = get_next_line(STDIN_FILENO);
		if (!line || ft_strncmp(delimiter, line, ft_strlen(delimiter)) == 0)
			break ;
		ft_putstr_fd(line, fd);
		free(line);
	}
	free(line);
	close(fd);
}

void	args_in(t_pipex *pipex, int argc, char **argv)
{
	if (ft_strncmp(argv[1], "here_doc", 9) != 0)
	{
		pipex->cmd_count = argc - 3;
	}
	else
	{
		pipex->cmd_count = argc - 4;
		pipex->here_doc = true;
	}
	if (pipex->cmd_count < 2)
		error("Not enough commands!", false);
	if (pipex->here_doc)
		read_stdin(argv[2]);
}