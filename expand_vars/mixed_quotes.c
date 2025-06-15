/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mixed_quotes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 02:54:30 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/04 11:43:46 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*process_single_quotes(char *content)
{
	size_t	len;

	if (content[0] == '\'' && content[ft_strlen(content) - 1] == '\'')
	{
		len = ft_strlen(content);
		if (len <= 2)
			return (ft_strdup(""));
		return (ft_strndup(&content[1], len - 2));
	}
	else
		return (ft_strdup(content));
}

static char	*process_double_quotes(char *content, t_shell *shell)
{
	char	*inner;
	char	*expanded;

	if (content[0] == '"' && content[ft_strlen(content) - 1] == '"')
	{
		inner = ft_strndup(&content[1], ft_strlen(content) - 2);
		expanded = expand_variables(inner, '"', shell);
		free(inner);
		return (expanded);
	}
	else
		return (expand_variables(content, '"', shell));
}

static char	*expand_segment(t_quote_segment *segment, t_shell *shell)
{
	if (!segment || !segment->content)
		return (ft_strdup(""));
	if (segment->quote_type == '\'')
		return (process_single_quotes(segment->content));
	else if (segment->quote_type == '"')
		return (process_double_quotes(segment->content, shell));
	else
		return (expand_variables(segment->content, '\0', shell));
}

static char	*join_expanded_segment(char *result, char *expanded_segment)
{
	char	*temp;

	if (!expanded_segment)
		return (result);
	temp = result;
	result = ft_strjoin(result, expanded_segment);
	free(temp);
	free(expanded_segment);
	return (result);
}

char	*expand_mixed_quotes(const char *str, t_shell *shell)
{
	t_quote_segment	*segments;
	int				count;
	char			*result;
	int				i;

	segments = parse_quote_segments(str, &count);
	if (!segments)
		return (ft_strdup(str));
	result = ft_strdup("");
	if (!result)
	{
		free(segments);
		return (NULL);
	}
	i = 0;
	while (i < count)
	{
		result = join_expanded_segment(result,
				expand_segment(&segments[i], shell));
		if (segments[i].content)
			free(segments[i].content);
		i++;
	}
	free(segments);
	return (result);
}
