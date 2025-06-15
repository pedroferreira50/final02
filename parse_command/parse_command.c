/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_command.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:42:33 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 18:22:42 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/// @brief Processes shell operators during command tokenization
/// @param state Parser state machine with current position and context
/// @param last_was_opera Pointer to flag tracking if 
/// previous token was operator
/// @param shell Global shell state for error reporting
/// @return 1 on success, 0 on syntax error
static int	handle_operator(t_parse *state, int *last_was_opera,
				t_shell *shell)
{
	char	op;
	int		is_double;

	op = state->cmd[state->i];
	is_double = (state->cmd[state->i + 1] == op && (op == '<' || op == '>'));
	if (is_double)
		state->i += 2;
	else
		state->i++;
	if ((op == '|' && *last_was_opera) || (op == '|' && !state->cmd[state->i]))
		return (error_operator(op, shell));
	if (*last_was_opera == 1 && op == '|')
		return (error_operator(op, shell));
	if (*last_was_opera && (op == '<' || op == '>') && *last_was_opera != 2)
		return (error_operator(op, shell));
	add_argument(state);
	state->operator_flags[state->args_count - 1] = '1';
	if (op == '|')
		*last_was_opera = 2;
	else
		*last_was_opera = 1;
	return (1);
}

static int	check_paren(t_parse *state, t_shell *shell,
		t_parse_result *result)
{
	if ((state->cmd[state->i] == '(' || state->cmd[state->i] == ')') && \
			!state->in_quotes)
	{
		print_error_simple("syntax error near token", 2, shell);
		free_state_args(state);
		result->args = NULL;
		result->quote_types = NULL;
		return (0);
	}
	return (1);
}

static void	parse_loop(t_parse *state, t_shell *shell,
		t_parse_result *result, int *last_was_operator)
{
	while (state->cmd[state->i])
	{
		while (ft_isspace(state->cmd[state->i]) && !state->in_quotes)
			state->i++;
		if (!state->cmd[state->i])
			break ;
		state->start = state->i;
		if (!check_paren(state, shell, result))
			return ;
		if (!state->in_quotes && (state->cmd[state->i] == '<' || \
				state->cmd[state->i] == '>' || state->cmd[state->i] == '|'))
		{
			if (!handle_operator(state, last_was_operator, shell))
			{
				*last_was_operator = 0;
				free_state_args(state);
				result->args = NULL;
				result->quote_types = NULL;
				return ;
			}
			continue ;
		}
		process_token(state, last_was_operator);
	}
}

static int	check_initial(const char *cmd, t_shell *shell, t_parse *state)
{
	if (!state->args || !state->quote_types)
	{
		if (shell->exit_status != 2)
			shell->exit_status = 1;
		return (0);
	}
	if (cmd && cmd[0] == '|')
	{
		print_error_token("|", 2, shell);
		return (0);
	}
	return (1);
}

/// @brief Main command parsing function - tokenizes input
/// with quote and operator handling
/// @param cmd Raw command string from user input
/// @param shell Global shell state for error handling
/// @return Parse result with tokenized args, quote types, and operator flags
t_parse_result	parse_command(char *cmd, t_shell *shell)
{
	t_parse			state;
	t_parse_result	result;
	int				last_was_operator;

	last_was_operator = 0;
	result.args = NULL;
	result.quote_types = NULL;
	result.operator_flags = NULL;
	initialize_state(&state, cmd);
	if (!check_initial(cmd, shell, &state))
		return (free_state(&state), result);
	parse_loop(&state, shell, &result, &last_was_operator);
	if (!check_errors(&state, shell, last_was_operator))
		return (free_state(&state), result);
	finalize_result(&state, &result, cmd);
	return (result);
}
