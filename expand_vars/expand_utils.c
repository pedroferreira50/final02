/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:46:39 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/09 17:43:59 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	find_quote_end(const char *str, int start, char quote_char)
{
	int	i;

	i = start + 1;
	while (str[i] && str[i] != quote_char)
		i++;
	return (i);
}

static void	skip_quoted_section(const char *str, int *i)
{
	char	quote_char;

	quote_char = str[*i];
	*i = find_quote_end(str, *i, quote_char);
	if (str[*i] == quote_char)
		(*i)++;
}

int	count_segments(const char *str)
{
	int	i;
	int	seg_count;

	if (!str)
		return (0);
	i = 0;
	seg_count = 0;
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
		{
			skip_quoted_section(str, &i);
			seg_count++;
		}
		else
		{
			while (str[i] && str[i] != '\'' && str[i] != '"')
				i++;
			seg_count++;
		}
	}
	return (seg_count);
}

size_t	get_var_len(const char *str, size_t i, t_var *vars, char **envp)
{
	size_t	var_len;
	char	*var_name;
	char	*var_value;

	if (!str)
		return (0);
	var_len = 0;
	while (str[i + 1 + var_len] && (isalnum(str[i + 1 + var_len])
			|| str[i + 1 + var_len] == '_'))
		var_len++;
	if (var_len == 0)
		return (0);
	var_name = ft_strndup(&str[i + 1], var_len);
	if (!var_name)
		return (0);
	var_value = get_var_value_helper(var_name, vars, envp);
	var_len = ft_strlen(var_value);
	free(var_name);
	return (var_len);
}

int	is_heredoc_delimiter(char **tokens, char *quote_types, int index)
{
	if (index > 0 && ft_strcmp(tokens[index - 1], "<<") == 0
		&& quote_types[index - 1] == '\0')
		return (1);
	return (0);
}

/* void	skip_unquoted_section(const char *str, int *i)
{
	while (str[*i] && str[*i] != '\'' && str[*i] != '"')
		(*i)++;
}

int	count_segments(const char *str)
{
	int	i;
	int	seg_count;

	if (!str)
		return (0);
	i = 0;
	seg_count = 0;
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
			skip_quoted_section(str, &i);
		else
			skip_unquoted_section(str, &i);
		seg_count++;
	}
	return (seg_count);
} */
