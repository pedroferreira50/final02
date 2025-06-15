/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:19:14 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 14:39:23 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	has_mixed_quotes_in_delimiter(char *delim, size_t len)
{
	size_t	i;
	int		quote_count;

	quote_count = 0;
	i = 0;
	while (i < len)
	{
		if (delim[i] == '"' || delim[i] == '\'')
			quote_count++;
		i++;
	}
	return (quote_count > 2);
}

char	*process_quoted_delimiter(char *delim, size_t len)
{
	if (len >= 2 && (delim[0] == '"' || delim[0] == '\'')
		&& delim[len - 1] == delim[0]
		&& !has_mixed_quotes_in_delimiter(delim, len))
	{
		return (ft_strndup(delim + 1, len - 2));
	}
	return (NULL);
}

char	*clean_mixed_quotes(char *delim, size_t len)
{
	char	*clean_delim;
	size_t	j;
	size_t	i;

	clean_delim = malloc(len + 1);
	if (!clean_delim)
		return (NULL);
	j = 0;
	i = 0;
	while (i < len)
	{
		if (delim[i] != '\'' && delim[i] != '"')
		{
			clean_delim[j] = delim[i];
			j++;
		}
		i++;
	}
	clean_delim[j] = '\0';
	return (clean_delim);
}

void	set_heredoc_delimiter(t_command_data *data, char *delim, int cmd_index)
{
	char	*processed_delim;
	char	*temp_delim;

	if (ft_strchr(delim, '\'') || ft_strchr(delim, '"'))
	{
		temp_delim = ft_strdup(delim);
		processed_delim = remove_quotes(temp_delim);
		free(temp_delim);
		if (processed_delim)
		{
			data->heredoc_delims[cmd_index] = processed_delim;
			data->heredoc_quoted[cmd_index] = 1;
		}
		else
		{
			data->heredoc_delims[cmd_index] = ft_strdup(delim);
			data->heredoc_quoted[cmd_index] = 0;
		}
	}
	else
	{
		data->heredoc_delims[cmd_index] = ft_strdup(delim);
		data->heredoc_quoted[cmd_index] = 0;
	}
}

int	validate_heredoc_args(char **args, t_indices *indices, t_shell *shell)
{
	if (args[indices->i + 1] == NULL)
	{
		print_error_simple("syntax error: no delimiter after '<<'", 2, shell);
		indices->i++;
		return (-1);
	}
	return (0);
}
