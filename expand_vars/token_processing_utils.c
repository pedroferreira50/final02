/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_processing_utils.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 17:00:00 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/09 17:00:00 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	has_unquoted_vars(const char *token)
{
	int		i;
	int		in_quotes;
	char	quote_char;

	i = 0;
	in_quotes = 0;
	quote_char = '\0';
	while (token[i])
	{
		if (!in_quotes && (token[i] == '"' || token[i] == '\''))
		{
			in_quotes = 1;
			quote_char = token[i];
		}
		else if (in_quotes && token[i] == quote_char)
		{
			in_quotes = 0;
			quote_char = '\0';
		}
		else if (!in_quotes && token[i] == '$' && token[i + 1]
			&& (ft_isalpha(token[i + 1]) || token[i + 1] == '_'))
			return (1);
		i++;
	}
	return (0);
}

static int	should_split_mixed_quotes(const char *original,
		const char *expanded)
{
	int	i;

	if (!original || !expanded)
		return (0);
	if (!has_unquoted_vars(original))
		return (0);
	i = 0;
	while (expanded[i])
	{
		if (ft_isspace(expanded[i]))
			return (1);
		i++;
	}
	return (0);
}

int	should_split_this_token(char **tokens, char *quote_types, int i,
		char *expanded_token)
{
	int		j;
	char	*token;

	token = tokens[i];
	if (!token)
		return (0);
	j = 0;
	while (token[j] && token[j] != '=')
	{
		if (!ft_isalnum(token[j]) && token[j] != '_')
			break ;
		j++;
	}
	if (token[j] == '=')
		return (0);
	if (should_split_token(quote_types[i], expanded_token))
		return (1);
	if (has_mixed_quotes(token)
		&& should_split_mixed_quotes(token, expanded_token))
		return (1);
	return (0);
}

void	add_split_tokens(char ***expanded, int *out_idx,
		char *expanded_token)
{
	char	**split_words;
	int		j;

	split_words = split_expanded_token(expanded_token);
	if (split_words)
	{
		j = 0;
		while (split_words[j])
		{
			(*expanded)[(*out_idx)++] = ft_strdup(split_words[j]);
			j++;
		}
		j = 0;
		while (split_words[j])
			free(split_words[j++]);
		free(split_words);
	}
	else if (expanded_token && !*expanded_token)
	{
		(*expanded)[(*out_idx)++] = ft_strdup("");
	}
}

void	process_single_expanded_token(char **tokens, char *quote_types,
		t_shell *shell, t_token_process_params *params)
{
	params->expanded_token = expand_single_token(tokens[params->i],
			quote_types[params->i], shell,
			is_heredoc_delimiter(tokens, quote_types, params->i));
	if (should_split_this_token(tokens, quote_types, params->i,
			params->expanded_token))
		add_split_tokens(&params->expanded, &params->out_idx,
			params->expanded_token);
	else
		params->expanded[params->out_idx++] = ft_strdup(params->expanded_token);
	free(params->expanded_token);
	params->i++;
}
