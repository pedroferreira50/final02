/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 07:26:00 by pedro             #+#    #+#             */
/*   Updated: 2025/06/11 04:54:57 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*decode_per_char_loop(char *result,
		const char *t, size_t j, size_t len)
{
	size_t	i;
	size_t	k;

	i = 0;
	while (i < len)
	{
		if (i + 3 < len && t[i] == 3 && t[i + 1] == 'P' && t[i + 2] == '"')
		{
			k = i + 3;
			while (k < len && t[k] != '"')
				k++;
			if (k + 1 < len && t[k] == '"' && t[k + 1] == 'P')
			{
				while (i + 3 < k)
					result[j++] = t[3 + i++];
				i = k + 2;
				continue ;
			}
		}
		result[j++] = t[i++];
	}
	result[j] = '\0';
	return (result);
}

static char	*decode_per_char_encoding(const char *token)
{
	char	*result;
	size_t	len;
	size_t	j;

	len = ft_strlen(token);
	result = malloc(len + 1);
	if (!result)
		return (ft_strdup(token));
	j = 0;
	return (decode_per_char_loop(result, token, j, len));
}

char	*clean_escaped_token(const char *token)
{
	char	*result;
	char	*temp;
	size_t	len;

	if (!token)
		return (NULL);
	if (ft_strchr(token, 3))
	{
		return (decode_per_char_encoding(token));
	}
	if (token[0] == 3 && token[1] == 'P' && token[2] == '"')
	{
		len = ft_strlen(token);
		if (len > 4 && token[len - 1] == 'P')
		{
			temp = ft_strndup(token + 2, len - 4);
			result = remove_quotes(temp);
			free(temp);
			if (!result)
				result = ft_strdup("");
			return (result);
		}
	}
	return (ft_strdup(token));
}

/// @brief Processes filename tokens by removing operator protection encoding
/// @param token Input token potentially
///containing 3P"..."P protection encoding
/// @return Cleaned filename string with quotes removed, 
/// or copy of original token
char	*process_filename_token(const char *token)
{
	char	*result;
	char	*temp;
	size_t	len;

	if (token && token[0] == 3 && token[1] == 'P' && token[2] == '"')
	{
		len = ft_strlen(token);
		if (len > 4 && token[len - 1] == 'P')
		{
			temp = ft_strndup(token + 2, len - 4);
			result = remove_quotes(temp);
			free(temp);
			if (!result)
				result = ft_strdup("");
			return (result);
		}
	}
	return (ft_strdup(token));
}

/// @brief Main parsing entry point - converts token array to
/// command pipeline structure
/// @param args Array of expanded and filtered argument tokens
/// @param count Number of tokens in args array
/// @param data Command data structure to populate
/// @param shell Global shell state for error handling
void	parse_input(char **args, int count, t_command_data *data,
	t_shell *shell)
{
	int				*arg_counts;
	t_count_context	ctx;

	ft_memset(data, 0, sizeof(t_command_data));
	arg_counts = initialize_arg_counts(count, shell);
	if (arg_counts == NULL)
		return ;
	ctx = (t_count_context){args, count, data, arg_counts, shell};
	count_commands(&ctx);
	if (data->num_commands == 0)
	{
		free_command_data(data);
		free(arg_counts);
		return ;
	}
	alloc_commands(data, shell);
	if (data->commands == NULL || data->arguments == NULL)
	{
		free(arg_counts);
		return ;
	}
	populate_commands(args, arg_counts, data, shell);
	finalize_arguments(data, arg_counts);
	free(arg_counts);
}
