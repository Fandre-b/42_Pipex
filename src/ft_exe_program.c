/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exe_program.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fandre-b <fandre-b@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 18:43:01 by fandre-b          #+#    #+#             */
/*   Updated: 2024/04/26 18:43:01 by fandre-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	process_child_pipes(t_info *info, int n)
{
	int		status;
	int		i;
	int		ch_finish;

	i = 0;
	while (1)
	{
		i = i % (n + 1);
		ch_finish = waitpid(info->pipeline[i][0], &status, WNOHANG);
		if (ch_finish > 0 && WIFEXITED(status))
		{
			while (n >= i)
			{
				if (close(info->pipeline[n][1]) == -1)
					return (perror("close streamline failed"), errno);
				if (close(info->pipeline[n][2]) == -1)
					return (perror("close streamline failed"), errno);
				n--;
			}
			if (n + 1 == 0)
				break ;
		}
		i++;
	}
	return (status);
}

void	exe_cmd_child(t_info *info, char **cmd)
{
	int	status;

	status = 0;
	if (info->exe_fd[0] != STDIN_FILENO)
		dup2(info->exe_fd[0], STDIN_FILENO);
	if (info->exe_fd[1] != STDOUT_FILENO)
		dup2(info->exe_fd[1], STDOUT_FILENO);
	if (close(info->exe_fd[0]) == -1 || close(info->exe_fd[1]) == -1)
		perror("close child exe_fds failed");
	close_all();
	if (!cmd[0] || (cmd[0] && access(cmd[0], F_OK | X_OK) != 0))
	{
		write(STDERR_FILENO, "Command not found: ", 19);
		if (cmd[0])
			write(STDERR_FILENO, cmd[0], ft_strlen(cmd[0]));
		write(STDERR_FILENO, "\n", 1);
		status = 127;
	}
	else if (execve(cmd[0], cmd, info->envp) == -1)
		status = errno;
	free_info(info);
	exit (status);
}

int	execute_command(t_info *info, char **cmd, int n)
{
	pid_t	pid;
	int		status;

	status = 0;
	n = info->size - n;
	pid = fork();
	if (pid == -1)
		return (perror("fork failed"), errno);
	else if (pid == 0)
		exe_cmd_child(info, cmd);
	else
	{
		info->pipeline[n][0] = pid;
		info->pipeline[n][1] = info->exe_fd[0];
		info->pipeline[n][2] = info->exe_fd[1];
	}
	return (status);
}
