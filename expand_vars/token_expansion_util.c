/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_expansion_util.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 21:31:09 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/10 21:59:54 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*encode_operators_into_buffer(char *result, char *str, size_t len);

static char	*replace_and_protect(char *old, char *new_val, int *was_protected)
{
	free(old);
	if (!new_val)
		return (ft_strdup(""));
	*was_protected = 1;
	return (new_val);
}

char	*protect_operators_and_expand_tilde(char *expanded, char *token,
		char quote_type, t_shell *shell)
{
	char	*tmp;
	int		was_protected;

	was_protected = 0;
	if (is_operator_small(expanded, token, quote_type))
	{
		tmp = expanded;
		expanded = replace_and_protect(tmp,
				quote_wrap_operator(tmp), &was_protected);
	}
	else if (contains_operators_from_variable(expanded, token, quote_type))
	{
		tmp = expanded;
		expanded = replace_and_protect(tmp,
				encode_operators_in_string(tmp), &was_protected);
	}
	if (!expanded)
		expanded = ft_strdup("");
	if (quote_type != '\'' && !was_protected)
	{
		tmp = expanded;
		expanded = expand_tilde(tmp, shell);
		free(tmp);
	}
	return (expanded);
}

char	*quote_wrap_operator(char *str)
{
	char	*quoted;
	size_t	len;

	len = ft_strlen(str);
	quoted = malloc(len + 6);
	if (!quoted)
		return (str);
	quoted[0] = 3;
	quoted[1] = 'P';
	quoted[2] = '"';
	ft_strcpy(quoted + 3, str);
	quoted[len + 3] = '"';
	quoted[len + 4] = 'P';
	quoted[len + 5] = '\0';
	return (quoted);
}

char	*encode_operators_in_string(char *str)
{
	char	*result;
	size_t	len;
	size_t	new_len;
	size_t	i;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	new_len = len;
	i = 0;
	while (i < len)
	{
		if (str[i] == '|' || str[i] == '<' || str[i] == '>')
			new_len += 5;
		i++;
	}
	result = malloc(new_len + 1);
	if (!result)
		return (str);
	return (encode_operators_into_buffer(result, str, len));
}

static char	*encode_operators_into_buffer(char *result, char *str, size_t len)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (i < len)
	{
		if (str[i] == '|' || str[i] == '<' || str[i] == '>')
		{
			result[j++] = 3;
			result[j++] = 'P';
			result[j++] = '"';
			result[j++] = str[i];
			result[j++] = '"';
			result[j++] = 'P';
		}
		else
			result[j++] = str[i];
		i++;
	}
	result[j] = '\0';
	return (result);
}
