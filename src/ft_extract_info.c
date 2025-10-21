/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_extract_info.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fandre-b <fandre-b@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 18:40:53 by fandre-b          #+#    #+#             */
/*   Updated: 2024/04/26 18:40:53 by fandre-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*get_path(char *cmd, char **envp)
{
	char	*temp;
	char	*paths;
	int		i_perm[2];

	i_perm[1] = -1;
	paths = NULL;
	temp = NULL;
	while (*envp != NULL && paths == NULL)
		paths = ft_strstr(*envp++, "PATH=");
	if (paths != NULL)
		paths = paths + 5;
	while (paths && *paths != '\0' && i_perm[1] != 0)
	{
		i_perm[0] = 0;
		free (temp);
		while (paths[i_perm[0]] && paths[i_perm[0]] != ':')
			i_perm[0]++;
		temp = ft_strnjoin(ft_strnjoin(NULL, paths, i_perm[0]++), "/", 1);
		temp = ft_strnjoin(temp, cmd, ft_strlen(cmd));
		i_perm[1] = access(temp, F_OK | X_OK);
		paths += i_perm[0];
	}
	if (i_perm[1] == 0)
		return (free (cmd), temp);
	return (free (temp), cmd);
}

int	malloc_struct(t_info *info, int i)
{
	info->cmd = (char ***) malloc(i * sizeof(char **));
	if (!info->cmd)
		return (perror("Malloc cmd failed"), errno);
	info->pipeline = (int **) malloc(--i * sizeof(int *));
	if (!info->pipeline)
		return (perror("Malloc pipe stream struct failed"), errno);
	info->size = i - 1;
	while (--i >= 0)
	{
		info->pipeline[i] = (int *) malloc (3 * sizeof(int));
		if (!info->pipeline[i])
			return (perror("malloc failed"), errno);
	}
	return (0);
}

int	parcel_argv(int argc, char **argv, t_info *info)
{
	int	i;

	info->here_doc = 0;
	i = (argc - 2);
	if (malloc_struct(info, i) != 0)
		return (-1);
	i = -1;
	while (++i + 3 + info->here_doc < argc)
	{
		info->cmd[i] = ft_split(argv[i + 2 + info->here_doc], ' ');
		if (info->cmd[i][0] && access(info->cmd[i][0], F_OK | X_OK) != 0)
			info->cmd[i][0] = get_path(info->cmd[i][0], info->envp);
		rejoin_quoted_args(info->cmd[i]);
	}
	info->cmd[i] = NULL;
	if (argc != 5)
		return (write(2, "Please use a valid number of arguments\n", 39), -1);
	return (0);
}

int	parcel_open_fd(int argc, char **argv, t_info *info)
{
	info->limiter = NULL;
	info->fd[0] = open(argv[1], O_RDONLY, 0666);
	if (-1 == info->fd[0])
		return (perror(argv[1]), -1);
	info->fd[1] = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (-1 == info->fd[1])
		return (perror(argv[argc - 1]), -1);
	return (0);
}
