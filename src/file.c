/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   file.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: lsinke <lsinke@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/10 12:22:27 by lsinke        #+#    #+#                 */
/*   Updated: 2022/05/10 12:22:27 by lsinke        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include "libft.h"
#include "get_next_line.h"
#include <fcntl.h>

/**
 * Create a temporary file, and read stdin into it, up to the delimiter
 */
static int	create_here_doc(const char *delimiter)
{
	int		fd;
	char	*line;

	fd = open(HERE_DOC, O_CREAT | O_RDWR | O_TRUNC, 00644);
	if (fd < 0)
		return (-1);
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
	return (fd);
}

/**
 * Opens the in- and output files.
 *
 * If we're dealing with a heredoc, read stdin into it.
 */
void	open_files(t_pipex *pipex, int argc, char **argv)
{
	int	out_flags;

	if (!pipex->here_doc)
	{
		out_flags = O_CREAT | O_WRONLY | O_TRUNC;
		pipex->fds[0] = open(argv[1], O_RDONLY);
	}
	else
	{
		out_flags = O_CREAT | O_WRONLY | O_APPEND;
		pipex->fds[0] = create_here_doc(argv[2]);
	}
	pipex->fds[pipex->fd_count - 1] = open(argv[argc - 1], out_flags, 00644);
	if (pipex->fds[0] < 0 || pipex->fds[pipex->fd_count - 1] < 0)
	{
		cleanup(pipex, true);
		error(ERROR_FILES, true);
	}
}

/**
 * Open a pipe.
 * After opening, swap it around so the fds are ordered in - out - in - out...
 */
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

/**
 * Open enough pipes for all commands
 */
void	open_pipes(t_pipex *pipex)
{
	int	pipe_index;

	pipe_index = 1;
	while (pipe_index < pipex->fd_count - 1)
	{
		if (!open_pipe(pipex->fds + pipe_index))
		{
			cleanup(pipex, true);
			error(ERROR_PIPE, true);
		}
		pipe_index += 2;
	}
}
