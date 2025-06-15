/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_commands.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 22:04:40 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/10 23:10:11 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	allocate_command_arrays(t_command_data *data)
{
	data->commands = malloc(data->num_commands * sizeof(char *));
	data->arguments = malloc(data->num_commands * sizeof(char **));
	data->heredoc_delims = malloc(data->num_commands * sizeof(char *));
	data->input_files = malloc(data->num_commands * sizeof(char *));
	data->out_redirs = malloc(data->num_commands * sizeof(t_redirection *));
	data->num_out_redirs = malloc(data->num_commands * sizeof(int));
	data->heredoc_quoted = malloc(data->num_commands * sizeof(int));
	data->heredoc_fds = malloc(data->num_commands * sizeof(int));
	return (data->commands && data->arguments
		&& data->heredoc_delims && data->input_files
		&& data->out_redirs && data->num_out_redirs
		&& data->heredoc_quoted && data->heredoc_fds);
}

void	alloc_commands(t_command_data *data, t_shell *shell)
{
	int	i;

	if (!allocate_command_arrays(data))
	{
		if (shell->exit_status != 2)
			shell->exit_status = 1;
		free_command_data(data);
		return ;
	}
	ft_memset(data->commands, 0, data->num_commands * sizeof(char *));
	ft_memset(data->arguments, 0, data->num_commands * sizeof(char **));
	ft_memset(data->heredoc_delims, 0, data->num_commands * sizeof(char *));
	ft_memset(data->input_files, 0, data->num_commands * sizeof(char *));
	ft_memset(data->out_redirs, 0,
		data->num_commands * sizeof(t_redirection *));
	ft_memset(data->num_out_redirs, 0, data->num_commands * sizeof(int));
	ft_memset(data->heredoc_quoted, 0, data->num_commands * sizeof(int));
	i = 0;
	while (i < data->num_commands)
	{
		data->heredoc_fds[i] = -1;
		i++;
	}
}

void	populate_command(char **args, int *arg_counts,
	t_command_data *data, t_parse_state *state)
{
	if (state->idx.j == 0)
	{
		data->commands[state->command_index] = clean_escaped_token(
				args[state->idx.i]);
		data->arguments[state->command_index] = malloc((arg_counts[
					state->command_index] + 1) * sizeof(char *));
		if (data->commands[state->command_index] == NULL
			|| data->arguments[state->command_index] == NULL)
			return ;
	}
}

void	populate_argument(char **args, t_command_data *data,
	t_parse_state *state)
{
	if (state->idx.j == 0 && args[state->idx.i] && !*args[state->idx.i])
	{
		while (args[state->idx.i] && !*args[state->idx.i])
			state->idx.i++;
		if (!args[state->idx.i])
			return ;
		if (data->commands[state->command_index])
			free(data->commands[state->command_index]);
		data->commands[state->command_index] = clean_escaped_token(
				args[state->idx.i]);
	}
	data->arguments[state->command_index][state->idx.j]
		= clean_escaped_token(args[state->idx.i]);
	if (data->arguments[state->command_index][state->idx.j] == NULL)
		return ;
	state->idx.j++;
	state->idx.i++;
}
