/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_expansion.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 16:45:00 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/11 03:09:01 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*expand_and_remove_quotes(char *token,
			char quote_type, t_shell *shell)
{
	char	*expanded;
	char	*tmp;

	if (quote_type != '\'')
		expanded = expand_variables(token, quote_type, shell);
	else
		expanded = ft_strdup(token);
	if (!expanded)
		expanded = ft_strdup(token);
	if (quote_type != '\0' || (quote_type == '\0' && !ft_strchr(token, '$')))
	{
		tmp = expanded;
		expanded = remove_quotes(tmp);
		free(tmp);
		if (!expanded)
			expanded = ft_strdup("");
	}
	return (expanded);
}

int	is_operator_small(char *expanded, char *token, char quote_type)
{
	if (quote_type == '\'' || quote_type == '"')
		return (0);
	if (token && (token[0] == '\'' || token[0] == '"'))
		return (0);
	if (expanded && (ft_strchr(expanded, '\'') || ft_strchr(expanded, '"')))
		return (0);
	return (ft_strchr(token, '$') && expanded
		&& (ft_strcmp(expanded, "<<") == 0
			|| ft_strcmp(expanded, ">>") == 0
			|| ft_strcmp(expanded, "<") == 0
			|| ft_strcmp(expanded, ">") == 0
			|| ft_strcmp(expanded, "|") == 0));
}

static int	is_pure_variable_expansion(char *token)
{
	int	i;

	if (!token || token[0] != '$')
		return (0);
	i = 1;
	if (!ft_isalpha(token[i]) && token[i] != '_')
		return (0);
	while (token[i] && (ft_isalnum(token[i]) || token[i] == '_'))
		i++;
	return (token[i] == '\0');
}

int	contains_operators_from_variable(char *expanded,
		char *token, char quote_type)
{
	if (!expanded || !token)
		return (0);
	if (is_pure_variable_expansion(token))
		return (ft_strchr(expanded, '|') || ft_strchr(expanded, '<')
			|| ft_strchr(expanded, '>'));
	if ((quote_type == '"' && (token[0] == '"' || ft_strchr(token, '"')))
		|| (quote_type == '\'' && (token[0] == '\''
				|| ft_strchr(token, '\''))))
		return (ft_strchr(expanded, '|') || ft_strchr(expanded, '<')
			|| ft_strchr(expanded, '>'));
	return (0);
}

char	*expand_single_token(char *token, char quote_type,
		t_shell *shell, int is_heredoc)
{
	char	*expanded;

	if (is_heredoc)
		return (ft_strdup(token));
	if (has_mixed_quotes(token))
	{
		expanded = expand_mixed_quotes(token, shell);
		if (!expanded)
			expanded = ft_strdup("");
		return (expanded);
	}
	expanded = expand_and_remove_quotes(token, quote_type, shell);
	expanded = protect_operators_and_expand_tilde(expanded,
			token, quote_type, shell);
	return (expanded);
}
