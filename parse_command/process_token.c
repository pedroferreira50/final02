/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_token.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:35:24 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 17:14:22 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/// @brief Handles quote character processing during tokenization
/// @param state Parser state with current position and quote context
static void	handle_quotes(t_parse *state)
{
	if ((state->cmd[state->i] == '\'' || state->cmd[state->i] == '"') && \
			!state->in_quotes)
	{
		state->quote_char = state->cmd[state->i];
		state->in_quotes = 1;
	}
	else if (state->cmd[state->i] == state->quote_char && \
				state->in_quotes && state->brace_count == 0)
	{
		state->in_quotes = 0;
		state->quote_char = '\0';
	}
}

/// @brief Handles brace character processing during 
/// tokenization (unused feature)
/// @param state Parser state with current position
static void	handle_braces(t_parse *state)
{
	if (state->in_quotes)
	{
		if (state->cmd[state->i] == '{')
			state->brace_count++;
		else if (state->cmd[state->i] == '}')
			state->brace_count--;
		state->i++;
	}
}

/// @brief Main token processing function during command parsing
/// @param state Parser state machine with current position and context
/// @param last_was_operator Pointer to flag tracking previous token type
void	process_token(t_parse *state, int *last_was_operator)
{
	int	had_quotes;

	had_quotes = 0;
	while (state->cmd[state->i] && (!ft_isspace(state->cmd[state->i]) \
		|| state->in_quotes))
	{
		if ((state->cmd[state->i] == '\'' || state->cmd[state->i] == '"')
			&& !state->in_quotes)
			had_quotes = 1;
		handle_quotes(state);
		if (!state->in_quotes && (state->cmd[state->i] == '<' || \
				state->cmd[state->i] == '>' || state->cmd[state->i] == '|'))
			break ;
		if (state->in_quotes)
			handle_braces(state);
		else
			state->i++;
	}
	if (state->i > state->start || had_quotes)
	{
		add_argument(state);
		*last_was_operator = 0;
	}
}

void	free_state_args(t_parse *state)
{
	int	i;

	i = 0;
	while (state->args[i])
	{
		free(state->args[i]);
		state->args[i] = NULL;
		i++;
	}
}
