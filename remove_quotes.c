/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   remove_quotes.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 03:47:00 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/04 03:47:00 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_quote_char(char c, char *quote_char, int *i)
{
	if ((c == '\'' || c == '"') && *quote_char == '\0')
	{
		*quote_char = c;
		(*i)++;
		return (1);
	}
	if (c == *quote_char && *quote_char != '\0')
	{
		*quote_char = '\0';
		(*i)++;
		return (1);
	}
	return (0);
}

char	*remove_quotes(char *str)
{
	char	*new_str;
	int		i;
	int		j;
	char	quote_char;

	if (!str)
		return (NULL);
	new_str = malloc(ft_strlen(str) + 1);
	if (!new_str)
		return (NULL);
	i = 0;
	j = 0;
	quote_char = '\0';
	while (str[i])
	{
		if (handle_quote_char(str[i], &quote_char, &i))
			continue ;
		new_str[j++] = str[i++];
	}
	new_str[j] = '\0';
	return (new_str);
}
