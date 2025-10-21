/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pipex.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fandre-b <fandre-b@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 18:49:51 by fandre-b          #+#    #+#             */
/*   Updated: 2024/04/26 18:49:51 by fandre-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	close_all(void)
{
	int	i;

	i = 2;
	while (++i < 250)
	{
		while (close(i) == -1)
		{
			if (errno == EBADF)
				break ;
			else if (errno != EINTR)
				return (errno);
		}
	}
	return (0);
}

void	free_info(t_info *info)
{
	int	i;
	int	j;

	i = -1;
	while (info->cmd[++i])
	{
		j = 0;
		while (info->cmd[i][j] != NULL)
			free (info->cmd[i][j++]);
		free (info->cmd[i][j]);
		free (info->cmd[i]);
		free (info->pipeline[i]);
	}
	free (info->cmd[i]);
	free (info->cmd);
	free (info->pipeline);
	free (info);
	return ;
}

int	pipe_arg_cmd(t_info *info)
{
	int		fd[2];
	int		n;

	n = -1;
	info->exe_fd[0] = info->fd[0];
	while (info->cmd[++n] != NULL)
	{
		if (info->cmd[n + 1] != NULL)
		{
			if (pipe(fd) == -1)
				return (perror("pipe failed"), -1);
			info->exe_fd[1] = fd[1];
			execute_command(info, info->cmd[n], n);
			info->exe_fd[0] = fd[0];
		}
		else if (info->cmd[n + 1] == NULL)
		{
			info->exe_fd[1] = info->fd[1];
			execute_command(info, info->cmd[n], n);
		}
	}
	return (process_child_pipes(info, n - 1));
}

int	main(int argc, char **argv, char **envp)
{
	t_info	*info;
	int		status;

	status = 0;
	if (argc < 4)
		return (-1);
	info = (t_info *) malloc(sizeof(t_info));
	if (!info)
		return (perror("malloc struct failed"), -1);
	info->envp = envp;
	if (parcel_argv(argc, argv, info) == -1)
		status = 1;
	if (status == 0 && parcel_open_fd(argc, argv, info) == -1)
		status = 1;
	if (status == 0)
		status = pipe_arg_cmd(info);
	free_info(info);
	return (status);
}
