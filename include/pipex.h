#ifndef PIPEX_H
# define PIPEX_H
# include <unistd.h>
# include <stdbool.h>

typedef struct s_pipex {
	int		*fds;
	int		fd_count;
	int		cmd_count;
	char	**path;
}	t_pipex;

void	error(char *msg, bool in_lib);
void	child(t_pipex *pipex, int *fds, char *cmd, char **envp);

void	close_pipes(t_pipex *pipex);
void	cleanup_fds(t_pipex *pipex);
void	cleanup(t_pipex *pipex, bool close_files);
void	cleanup_child(t_pipex *pipex, char **argv);

#endif
