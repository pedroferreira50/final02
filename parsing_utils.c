/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 14:39:03 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 17:02:42 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_args(char **args)
{
	int	count;

	count = 0;
	while (args[count])
	{
		count++;
	}
	return (count);
}

// state is parse not state struct
void	add_argument(t_parse *state)
{
	int		len;
	size_t	max_tokens;

	len = state->i - state->start;
	if (state->cmd != NULL)
		max_tokens = ft_strlen(state->cmd) / 2 + 2;
	else
		max_tokens = 4;
	if (state->args_count < 0 || state->args_count >= (int)(max_tokens - 1))
	{
		ft_putstr_fd("minishell: error: too many tokens\n", STDERR_FILENO);
		return ;
	}
	state->args[state->args_count] = ft_strndup(&state->cmd[state->start], len);
	if (state->cmd[state->start] == '"' || state->cmd[state->start] == '\'')
		state->quote_types[state->args_count] = state->cmd[state->start];
	else
		state->quote_types[state->args_count] = '\0';
	state->quote_types[state->args_count + 1] = '\0';
	state->args_count++;
	state->start = state->i;
}
