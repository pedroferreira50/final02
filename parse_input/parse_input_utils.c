/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_input_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 07:29:00 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/11 04:54:26 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/// @brief Checks if token is a shell operator (pipe or redirection)
/// @param token String token to check
/// @return 1 if token is |, <, >, >>, <<; 0 otherwise
int	is_operator(const char *token)
{
	if (token == NULL || *token == '\0')
		return (0);
	if (ft_strcmp(token, "|") == 0
		|| ft_strcmp(token, "<") == 0
		|| ft_strcmp(token, ">") == 0
		|| ft_strcmp(token, ">>") == 0
		|| ft_strcmp(token, "<<") == 0)
		return (1);
	return (0);
}

/// @brief Allocates and initializes argument count array for pipeline commands
/// @param count Total number of input tokens
/// @param shell Global shell state for error handling
/// @return Pointer to allocated integer array, NULL on failure
int	*initialize_arg_counts(int count, t_shell *shell)
{
	int	*arg_counts;

	arg_counts = malloc(count * sizeof(int));
	if (arg_counts == NULL)
	{
		if (shell->exit_status != 2)
			shell->exit_status = 1;
		return (NULL);
	}
	ft_memset(arg_counts, 0, sizeof(int) * count);
	return (arg_counts);
}

/// @brief Null-terminates all command argument arrays after population
/// @param data Command data structure with populated argument arrays
/// @param arg_counts Array tracking current argument count per command
void	finalize_arguments(t_command_data *data, int *arg_counts)
{
	int	i;

	i = 0;
	while (i < data->num_commands)
	{
		if (data->arguments[i] != NULL)
			data->arguments[i][arg_counts[i]] = NULL;
		i++;
	}
}

/// @brief Wrapper function for handling operators during command population
/// @param args Token array being processed
/// @param data Command data structure being populated  
/// @param state Parser state with indices and command tracking
/// @param shell Global shell state for error handling
static void	handle_operator_wrapper(char **args, t_command_data *data,
	t_parse_state *state, t_shell *shell)
{
	t_redirect_context	redirect_ctx;

	if (args[state->idx.i] == NULL)
		return ;
	if (ft_strcmp(args[state->idx.i], "|") == 0)
	{
		state->command_index++;
		state->idx.j = 0;
		state->idx.i++;
		return ;
	}
	redirect_ctx.args = args;
	redirect_ctx.data = data;
	redirect_ctx.indices = &state->idx;
	redirect_ctx.shell = shell;
	redirect_ctx.cmd_index = state->command_index;
	shell->is_counting = 0;
	if (ft_strcmp(args[state->idx.i], "<<") == 0)
		handle_heredoc(&redirect_ctx);
	else
		handle_redirect(&redirect_ctx);
	if (shell->exit_status == 2)
		return ;
}

/// @brief Populates command data structure by distributing
/// tokens to appropriate commands
/// @param args Array of parsed tokens to distribute
/// @param arg_counts Array tracking argument count per command
/// @param data Command data structure to populate
/// @param shell Global shell state for error handling
void	populate_commands(char **args, int *arg_counts,
	t_command_data *data, t_shell *shell)
{
	t_parse_state	state;
	size_t			arg_count;

	state.command_index = 0;
	state.idx.i = 0;
	state.idx.j = 0;
	arg_count = 0;
	if (args == NULL)
	{
		shell->exit_status = 2;
		return ;
	}
	arg_count = count_args(args);
	while (state.idx.i < arg_count && args[state.idx.i] != NULL)
	{
		if (is_operator(args[state.idx.i]))
			handle_operator_wrapper(args, data, &state, shell);
		else
		{
			populate_command(args, arg_counts, data, &state);
			populate_argument(args, data, &state);
		}
	}
}
