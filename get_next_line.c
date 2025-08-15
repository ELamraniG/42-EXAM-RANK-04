/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moel-amr <moel-amr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 18:55:12 by moel-amr          #+#    #+#             */
/*   Updated: 2024/11/16 13:32:58 by moel-amr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	ft_mew_line(char *str, int n, char **dst, char **rest)
{
	int		i;
	char	*tmp;

	i = 0;
	if (n == 1)
	{
		if (str == NULL || *str == 0)
			return (-1);
		while (str[i])
		{
			if (str[i] == '\n')
				return (i);
			i++;
		}
		return (-1);
	}
	else
	{
		tmp = *dst;
		*dst = ft_substr(*rest, 0, ft_strlen(*rest));
		free(tmp);
		free(*rest);
		*rest = NULL;
	}
	return (0);
}

int	ft_do_the_while(char **dst, char **rest, char **buffer)
{
	int		nl_pos;
	char	*tmp;
	char	*tmp2;

	nl_pos = ft_mew_line(*buffer, 1, NULL, NULL);
	if (nl_pos == -1)
	{
		tmp = *dst;
		*dst = ft_strjoin(*dst, *buffer);
		free(tmp);
	}
	else
	{
		tmp = *dst;
		tmp2 = ft_substr(*buffer, 0, nl_pos + 1);
		*dst = ft_strjoin(*dst, tmp2);
		free(tmp2);
		free(tmp);
		tmp = *rest;
		*rest = ft_substr(*buffer, nl_pos + 1, ft_strlen(*buffer) - nl_pos - 1);
		free(tmp);
		free(*buffer);
		return (-1);
	}
	return (0);
}

int	ft_get_rest(char **rest, char **dst)
{
	int		nl_pos;
	char	*tmp2;

	nl_pos = ft_mew_line(*rest, 1, NULL, NULL);
	if (nl_pos >= 0)
	{
		tmp2 = *rest;
		*dst = ft_substr(*rest, 0, nl_pos + 1);
		*rest = ft_substr(*rest, nl_pos + 1, ft_strlen(*rest) - nl_pos - 1);
		free(tmp2);
		return (1);
	}
	else
		return (0);
}

void	ft_read_next(int fd, char **dst, char **buffer, char **rest)
{
	int	bytesread;

	bytesread = read(fd, *buffer, BUFFER_SIZE);
	while ((bytesread) > 0)
	{
		(*buffer)[bytesread] = 0;
		if ((ft_do_the_while(dst, rest, buffer)) == -1)
			return ;
		bytesread = read(fd, *buffer, BUFFER_SIZE);
	}
	if (bytesread == -1)
	{
		free(*buffer);
		free(*rest);
		free(*dst);
		*dst = NULL;
		*buffer = NULL;
		*rest = NULL;
	}
	if (*dst && (*dst)[0] == 0)
	{
		free(*dst);
		*dst = NULL;
	}
	free(*buffer);
}

char	*get_next_line(int fd)
{
	char		*buffer;
	char		*dst;
	static char	*rest;

	dst = NULL;
	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	if (rest != NULL)
	{
		if (ft_get_rest(&rest, &dst) == 1)
			return (dst);
		else
			ft_mew_line(NULL, 2, &dst, &rest);
	}
	buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buffer)
	{
		free(rest);
		free(dst);
		return (NULL);
	}
	ft_read_next(fd, &dst, &buffer, &rest);
	return (dst);
}
