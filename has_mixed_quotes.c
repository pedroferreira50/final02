/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   has_mixed_quotes.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 04:08:00 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/04 04:08:52 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_dollar_quote_pattern(const char *str, int *i)
{
	if (str[*i] == '"' && *i > 0 && str[*i - 1] == '$')
	{
		(*i)++;
		while (str[*i] && str[*i] != '"')
			(*i)++;
		if (str[*i] == '"')
			(*i)++;
		return (1);
	}
	return (0);
}

static void	handle_quote_start(const char *str, int i, char *quote_char,
	int *flags)
{
	*quote_char = str[i];
	flags[2] = 1;
	if (str[i] == '\'')
		flags[0] = 1;
	else
		flags[1] = 1;
	if ((i > 0 && str[i - 1] == str[i]) || (str[i + 1] == str[i]))
		flags[3] = 1;
}

static void	handle_quote_end(const char *str, int i, char *quote_char,
	int *flags)
{
	flags[2] = 0;
	*quote_char = '\0';
	if (str[i + 1] == str[i])
		flags[3] = 1;
}

static int	check_mixed_quotes_and_content(const char *str, int i, int *flags)
{
	if (!flags[2] && str[i] != '\'' && str[i] != '"')
	{
		if (!(str[i] == '$' && str[i + 1] == '"'))
			flags[4] = 1;
	}
	if ((flags[0] && flags[1]) || flags[3]
		|| ((flags[0] || flags[1]) && flags[4]))
		return (1);
	return (0);
}

int	has_mixed_quotes(const char *str)
{
	int		flags[5];
	char	quote_char;
	int		i;

	i = 0;
	while (i < 5)
		flags[i++] = 0;
	quote_char = '\0';
	i = 0;
	while (str[i])
	{
		if ((str[i] == '\'' || str[i] == '"') && !flags[2])
		{
			if (handle_dollar_quote_pattern(str, &i))
				continue ;
			handle_quote_start(str, i, &quote_char, flags);
		}
		else if (str[i] == quote_char && flags[2])
			handle_quote_end(str, i, &quote_char, flags);
		if (check_mixed_quotes_and_content(str, i, flags))
			return (1);
		i++;
	}
	return (0);
}
