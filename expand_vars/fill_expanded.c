/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_expanded.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:44:40 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/05 20:00:58 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	handle_escape_sequence(char *dest, const char *src,
		t_indices *indices)
{
	if (src[indices->i + 1] == '$')
	{
		dest[indices->j++] = '$';
		indices->i += 2;
	}
	else if (src[indices->i + 1] == '\\')
	{
		dest[indices->j++] = '\\';
		indices->i += 2;
	}
	else
	{
		dest[indices->j++] = src[indices->i++];
		dest[indices->j++] = src[indices->i++];
	}
}

static void	handle_dollar_quote_case(char *dest, const char *src,
		t_indices *indices, t_shell *shell)
{
	size_t	start;
	size_t	end;

	start = indices->i + 2;
	end = start;
	while (src[end] && src[end] != '"')
		end++;
	if (src[end] == '"')
	{
		while (start < end)
			dest[indices->j++] = src[start++];
		indices->i = end + 1;
	}
	else
	{
		if (src[indices->i + 1] == '?')
			fill_exit_status(dest, indices, shell);
		else if (ft_isalpha(src[indices->i + 1]) || src[indices->i + 1] == '_')
			fill_var_name(dest, src, indices, shell);
		else
			dest[indices->j++] = src[indices->i++];
	}
}

static void	handle_dollar_sign(char *dest, const char *src, t_indices *indices,
	t_shell *shell)
{
	if (src[indices->i + 1] == '"')
	{
		handle_dollar_quote_case(dest, src, indices, shell);
	}
	else if (src[indices->i + 1] == '?')
		fill_exit_status(dest, indices, shell);
	else if (ft_isalpha(src[indices->i + 1]) || src[indices->i + 1] == '_')
		fill_var_name(dest, src, indices, shell);
	else
		dest[indices->j++] = src[indices->i++];
}

static void	process_character(char *dest, const char *src, t_indices *indices,
	t_shell *shell)
{
	if (src[indices->i] == '\\' && src[indices->i + 1])
	{
		handle_escape_sequence(dest, src, indices);
	}
	else if (src[indices->i] == '$')
	{
		handle_dollar_sign(dest, src, indices, shell);
	}
	else
	{
		dest[indices->j++] = src[indices->i++];
	}
}

void	fill_expanded(char *dest, const char *src, char quote_type,
		t_shell *shell)
{
	t_indices	indices;

	indices.i = 0;
	indices.j = 0;
	if (!dest || !src)
		return ;
	if (quote_type == '\'')
		return (fill_quotes(dest, src, &indices, quote_type));
	while (src[indices.i])
	{
		process_character(dest, src, &indices, shell);
	}
	dest[indices.j] = '\0';
}
