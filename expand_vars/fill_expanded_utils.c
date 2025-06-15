/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_expanded_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 07:01:36 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/10 20:10:24 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	fill_quotes(char *dest, const char *src, t_indices *indices,
	char quote_type)
{
	indices->i++;
	while (src[indices->i] && src[indices->i] != quote_type)
	{
		dest[indices->j++] = src[indices->i++];
	}
	if (src[indices->i] == quote_type)
		indices->i++;
	dest[indices->j] = '\0';
}

void	fill_exit_status(char *dest, t_indices *indices, t_shell *shell)
{
	size_t	len;

	len = handle_exit_status(&dest[indices->j], 1, &indices->i, shell);
	indices->j += len;
}

void	fill_var_name(char *dest, const char *src, t_indices *indices,
	t_shell *shell)
{
	size_t	start;
	char	*key;
	char	*val;

	if (!dest || !src || !indices || !shell)
		return ;
	indices->i++;
	start = indices->i;
	if (!isalpha(src[start]) && src[start] != '_')
		return ((void)(dest[indices->j++] = '$'));
	while (isalnum(src[indices->i]) || src[indices->i] == '_')
		indices->i++;
	if (indices->i == start)
		return ((void)(dest[indices->j++] = '$'));
	key = ft_strndup(&src[start], indices->i - start);
	if (!key)
		return ;
	val = get_var_value_helper(key, shell->vars, shell->envp);
	if (val)
	{
		ft_strcpy(&dest[indices->j], val);
		indices->j += ft_strlen(val);
	}
	free(key);
}
