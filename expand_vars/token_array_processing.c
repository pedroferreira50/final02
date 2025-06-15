/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_array_processing.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 16:45:00 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/09 16:45:00 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	count_split_words(char *expanded_token)
{
	char	**split_words;
	int		word_count;
	int		i;

	split_words = split_expanded_token(expanded_token);
	if (!split_words)
	{
		if (expanded_token && !*expanded_token)
			return (1);
		return (0);
	}
	word_count = 0;
	while (split_words[word_count])
		word_count++;
	i = 0;
	while (i < word_count)
		free(split_words[i++]);
	free(split_words);
	return (word_count);
}

static int	count_expanded_tokens(char **tokens,
		char *quote_types, t_shell *shell)
{
	int		total_count;
	int		i;
	char	*expanded_token;

	total_count = 0;
	i = 0;
	while (tokens[i])
	{
		expanded_token = expand_single_token(tokens[i], quote_types[i], shell,
				is_heredoc_delimiter(tokens, quote_types, i));
		if (should_split_this_token(tokens, quote_types, i, expanded_token))
			total_count += count_split_words(expanded_token);
		else
			total_count++;
		free(expanded_token);
		i++;
	}
	return (total_count);
}

char	**expand_tokens(char **tokens, char *quote_types, t_shell *shell)
{
	char					**expanded;
	int						total_count;
	t_token_process_params	params;

	if (!tokens || !tokens[0])
		return (NULL);
	total_count = count_expanded_tokens(tokens, quote_types, shell);
	expanded = malloc(sizeof(char *) * (total_count + 1));
	if (!expanded)
		return (NULL);
	params.expanded = expanded;
	params.i = 0;
	params.out_idx = 0;
	while (tokens[params.i])
		process_single_expanded_token(tokens, quote_types, shell, &params);
	expanded[params.out_idx] = NULL;
	return (expanded);
}
