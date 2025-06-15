/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   calc_size.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 06:57:40 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/11 04:53:19 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/// @brief Handles escape sequence processing during size calculation
/// @param arg Source string containing potential escape sequences
/// @param indices Current position indices for parsing
/// @return Size contribution of escape sequence (typically 1)
static size_t	handle_escape_sequences(const char *arg, t_indices *indices)
{
	if (arg[indices->i + 1] == '$')
	{
		indices->i += 2;
		return (1);
	}
	else if (arg[indices->i + 1] == '\\')
	{
		indices->i += 2;
		return (1);
	}
	else
	{
		indices->i += 2;
		return (2);
	}
}

/// @brief Handles quoted content during size calculation
/// (preserves literal characters)
/// @param arg Source string with quoted content
/// @param indices Current position indices for parsing
/// @return Size contribution of quoted content (1 character)
static size_t	handle_quoted_content(const char *arg, t_indices *indices)
{
	size_t	start;
	size_t	end;

	start = indices->i + 2;
	end = start;
	while (arg[end] && arg[end] != '"')
		end++;
	if (arg[end] == '"')
	{
		indices->i = end + 1;
		return (end - start);
	}
	return (0);
}

/// @brief Handles dollar sign expansion during size calculation ($VAR, $?)
/// @param arg Source string containing dollar expansion
/// @param indices Current position indices for parsing
/// @param shell Global shell state for variable lookup
/// @return Size needed for expanded variable value
static size_t	handle_dollar_expansion(const char *arg, t_indices *indices,
	t_shell *shell)
{
	size_t	quoted_size;

	if (arg[indices->i + 1] == '"')
	{
		quoted_size = handle_quoted_content(arg, indices);
		if (quoted_size > 0)
			return (quoted_size);
		else
			return (calc_var_size(arg, indices, shell));
	}
	else
	{
		return (calc_var_size(arg, indices, shell));
	}
}

/// @brief Processes character sequence counting size for variable expansion
/// @param arg Source string to analyze character by character
/// @param indices Current position indices for parsing state
/// @param shell Global shell state for variable lookup during expansion
/// @return Total size contribution of processed character sequence
static size_t	process_character_sequence(const char *arg, t_indices *indices,
	t_shell *shell)
{
	size_t	size;

	size = 0;
	while (arg[indices->i])
	{
		if (arg[indices->i] == '\\' && arg[indices->i + 1])
		{
			size += handle_escape_sequences(arg, indices);
			continue ;
		}
		if (arg[indices->i] == '$')
		{
			size += handle_dollar_expansion(arg, indices, shell);
			continue ;
		}
		size++;
		indices->i++ ;
	}
	return (size);
}

/// @brief Calculates total size needed for string after variable expansion
/// @param arg Source string to analyze for expansion
/// @param quote_type Quote context affecting expansion behavior
/// @param shell Global shell state for variable lookup and exit status
/// @return Total size in bytes needed for expanded string
/// including null terminator
size_t	calc_expanded_size(const char *arg, char quote_type, t_shell *shell)
{
	size_t		size;
	t_indices	indices;

	if (!arg || !shell)
		return (1);
	if (quote_type == '\'')
		return (ft_strlen(arg) + 1);
	indices.i = 0;
	indices.j = 0;
	size = process_character_sequence(arg, &indices, shell);
	return (size + 1);
}
