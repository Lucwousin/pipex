#include "pipex.h"
#include "libft.h"

void	close_pipes(t_pipex *pipex)
{
	int	i;

	i = 0;
	while (i < pipex->fd_count - 1)
		close(pipex->fds[i++]);
}

void	cleanup_fds(t_pipex *pipex)
{
	int	i;

	i = 0;
	while (i < pipex->fd_count)
	{
		if (pipex->fds[i] != 0)
			close(pipex->fds[i]);
		++i;
	}
	free(pipex->fds);
}

void	cleanup(t_pipex *pipex, bool close_files)
{
	if (close_files)
	{
		close(pipex->fds[0]);
		close(pipex->fds[pipex->fd_count - 1]);
	}
	free(pipex->fds);
	ft_free_mult((void **) pipex->path);
}

void	cleanup_child(t_pipex *pipex, char **argv)
{
	cleanup(pipex, false);
	ft_free_mult((void **) argv);
}

void	cleanup_heredoc(void)
{
	unlink(HERE_DOC);
}