/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   pipex.h                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: lsinke <lsinke@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/10 12:22:20 by lsinke        #+#    #+#                 */
/*   Updated: 2022/05/10 12:22:20 by lsinke        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H
# include <unistd.h>
# include <stdbool.h>
# define HERE_DOC		".pipex_here_doc"
# define ERROR_USAGE	"Usage: ./pipex <in> <cmd1> <cmd2> <cmdn>.. <out>"
# define ERROR_COMMANDS	"Not enough commands!"
# define ERROR_PID_MAL	"Failed to allocate pid array"
# define ERROR_FD_MAL	"Allocating file descriptor array failed"
# define ERROR_PATH_NF	"No PATH environment variable was found"
# define ERROR_PATH_MAL	"Allocating PATHs failed for some weird reason"
# define ERROR_FILES	"Error opening files"
# define ERROR_PIPE		"Error opening pipe"
# define ERROR_FORK		"Forking failed!"
# define ERROR_DUP2		"Redirecting file descriptors failed"
# define ERROR_CMD_NF	"Command not found!"
# define ERROR_EXEC		"Something went wrong during execve call"
# define EXIT_CODE_NOT_FOUND	127

typedef struct s_pipex {
	int		*fds;
	int		fd_count;
	int		cmd_count;
	pid_t	last_cmd_pid;
	char	**path;
	bool	here_doc;
}	t_pipex;

void	error(char *msg, bool in_lib);
void	error_exit_code(char *msg, bool in_lib, int exit_code);

void	open_files(t_pipex *pipex, int argc, char **argv);
void	open_pipes(t_pipex *pipex);
void	create_forks(t_pipex *pipex, char **argv, char **envp);
int		wait_for_children(t_pipex *pipex);

void	child(t_pipex *pipex, int *fds, char *cmd, char **envp);

void	cleanup(t_pipex *pipex, bool close_files);
void	cleanup_child(t_pipex *pipex, char **argv);
void	close_pipes(t_pipex *pipex);

#endif
