/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_command_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:36:00 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 16:59:01 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/// @brief Frees all allocated memory in parser state structure
/// @param state Parser state containing arrays to free
void	free_state(t_parse *state)
{
	if (state->args)
	{
		free_args(state->args, NULL);
		state->args = NULL;
	}
	if (state->quote_types)
	{
		free(state->quote_types);
		state->quote_types = NULL;
	}
	if (state->operator_flags)
	{
		free(state->operator_flags);
		state->operator_flags = NULL;
	}
}

/// @brief Initializes parser state structure with allocated memory
/// @param state Parser state structure to initialize
/// @param cmd Command string to parse (used for size estimation)
void	initialize_state(t_parse *state, char *cmd)
{
	size_t	max_tokens;

	max_tokens = 4;
	if (cmd != NULL)
		max_tokens = ft_strlen(cmd) / 2 + 2;
	ft_memset(state, 0, sizeof(t_parse));
	state->cmd = cmd;
	state->args = malloc(sizeof(char *) * max_tokens);
	state->quote_types = malloc(sizeof(char) * max_tokens);
	state->operator_flags = malloc(sizeof(char) * max_tokens);
	if (!state->args || !state->quote_types || !state->operator_flags)
	{
		free(state->args);
		free(state->quote_types);
		free(state->operator_flags);
		state->args = NULL;
		state->quote_types = NULL;
		state->operator_flags = NULL;
		return ;
	}
	ft_memset(state->args, 0, sizeof(char *) * max_tokens);
	ft_memset(state->quote_types, 0, sizeof(char) * max_tokens);
	ft_memset(state->operator_flags, 0, sizeof(char) * max_tokens);
}

/// @brief Checks if quoted redirection operators are being used as commands
/// @param state Parser state with parsed arguments
/// @param shell Global shell state for error reporting
/// @return 1 if invalid redirection usage found, 0 otherwise
static int	redir_isnot_command(t_parse *state, t_shell *shell)
{
	if ((ft_strcmp(state->args[0], "\">>\"") == 0)
		|| ft_strcmp(state->args[0], "\'>>\'") == 0)
	{
		shell->exit_status = 127;
		return (print_error_command(">>", "Command not found"));
	}
	if ((ft_strcmp(state->args[0], "\">\"") == 0)
		|| ft_strcmp(state->args[0], "\'>\'") == 0)
	{
		shell->exit_status = 127;
		return (print_error_command(">", "Command not found"));
	}
	if ((ft_strcmp(state->args[0], "\"<<\"") == 0)
		|| ft_strcmp(state->args[0], "\'>>\'") == 0)
	{
		shell->exit_status = 127;
		return (print_error_command("<<", "Command not found"));
	}
	if ((ft_strcmp(state->args[0], "\"<\"") == 0)
		|| ft_strcmp(state->args[0], "\'>\'") == 0)
	{
		shell->exit_status = 127;
		return (print_error_command("<", "Command "));
	}
	return (0);
}

/// @brief Validates parsed command for syntax errors and invalid constructs
/// @param state Parser state with completed parsing data
/// @param shell Global shell state for error reporting and exit status
/// @param last_was_operator Flag indicating if last token was an operator
/// @return 1 if parsing is valid, 0 if errors found
int	check_errors(t_parse *state, t_shell *shell, int last_was_operator)
{
	if (state->args[0] == NULL)
		return (0);
	if (state->in_quotes)
	{
		print_error_simple("syntax error: unclosed quotes", 2, shell);
		return (0);
	}
	if (redir_isnot_command(state, shell))
		return (0);
	if (last_was_operator)
	{
		print_error_token("newline", 2, shell);
		return (0);
	}
	return (1);
}

/// @brief Transfers parser state data to parse result structure
/// @param state Parser state with completed parsing data
/// @param result Parse result structure to populate
/// @param cmd Original command string (unused)
void	finalize_result(t_parse *state, t_parse_result *result, const char *cmd)
{
	size_t	max_tokens;

	if (cmd)
		max_tokens = ft_strlen(cmd) / 2 + 2;
	else
		max_tokens = 4;
	if (state->args_count < (int)max_tokens)
		state->args[state->args_count] = NULL;
	result->args = state->args;
	result->quote_types = state->quote_types;
	result->operator_flags = state->operator_flags;
}
