/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   word_splitting.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 01:15:00 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/09 18:23:35 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	count_words(const char *str)
{
	int	count;
	int	in_word;
	int	i;

	if (!str)
		return (0);
	count = 0;
	in_word = 0;
	i = 0;
	while (str[i])
	{
		if (ft_isspace(str[i]))
		{
			in_word = 0;
		}
		else if (!in_word)
		{
			in_word = 1;
			count++;
		}
		i++;
	}
	return (count);
}

static char	*extract_word(const char *str, int *start)
{
	int		end;
	char	*word;

	while (str[*start] && ft_isspace(str[*start]))
		(*start)++;
	if (!str[*start])
		return (NULL);
	end = *start;
	while (str[end] && !ft_isspace(str[end]))
		end++;
	word = ft_strndup(&str[*start], end - *start);
	*start = end;
	return (word);
}

static char	**fill_words(const char *token, int word_count)
{
	char	**words;
	int		i;
	int		pos;

	i = 0;
	pos = 0;
	words = malloc(sizeof(char *) * (word_count + 1));
	if (!words)
		return (NULL);
	while (i < word_count)
	{
		words[i] = extract_word(token, &pos);
		if (!words[i])
		{
			while (--i >= 0)
				free(words[i]);
			free(words);
			return (NULL);
		}
		i++;
	}
	words[i] = NULL;
	return (words);
}

char	**split_expanded_token(const char *expanded_token)
{
	int		word_count;

	if (!expanded_token)
		return (NULL);
	word_count = count_words(expanded_token);
	if (word_count == 0)
		return (NULL);
	return (fill_words(expanded_token, word_count));
}

int	should_split_token(char quote_type, const char *expanded_token)
{
	int	i;

	if (quote_type == '\'')
		return (0);
	if (quote_type == '"')
		return (0);
	if (!expanded_token)
		return (0);
	i = 0;
	while (expanded_token[i])
	{
		if (ft_isspace(expanded_token[i]))
			return (1);
		i++;
	}
	return (0);
}
