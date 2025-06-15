/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_commands.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 17:49:46 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/14 01:58:31 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/// @brief Initializes execution state and allocates PID array for pipeline
/// @param data Command data structure containing pipeline information
/// @param state Execution state to initialize
/// @param shell Global shell state for error handling
/// @return Allocated PID array for child processes, NULL on failure
pid_t	*init_execution(t_command_data *data, t_exec_state *state,
	t_shell *shell)
{
	pid_t	*pids;

	pids = NULL;
	if (data == NULL || data->commands == NULL || data->num_commands == 0)
	{
		shell->exit_status = 2;
		return (NULL);
	}
	pids = malloc(sizeof(pid_t) * data->num_commands);
	if (pids == NULL)
	{
		shell->exit_status = 1;
		return (NULL);
	}
	ft_memset(pids, 0, sizeof(pid_t) * data->num_commands);
	state->prev_pipe_read = -1;
	state->i = 0;
	return (pids);
}

void	execute_parent(t_command_data *data, t_exec_state *state,
	t_shell *shell)
{
	if (ft_strcmp(data->commands[state->i], "cd") == 0)
	{
		ft_cd(data->arguments[state->i], &state->i, shell);
		shell->is_save_to_execute = false;
	}
	else if (ft_strcmp(data->commands[state->i], "export") == 0)
	{
		shell->exit_status = ft_export(data->arguments[state->i], shell);
		shell->is_save_to_execute = false;
	}
	else if (ft_strcmp(data->commands[state->i], "unset") == 0)
	{
		shell->exit_status = ft_unset(data->arguments[state->i], \
				&shell->vars, &shell->envp);
		shell->is_save_to_execute = false;
	}
	else if (ft_strcmp(data->commands[state->i], "exit") == 0)
	{
		ft_exit(data->arguments[state->i], shell, data);
		shell->is_save_to_execute = false;
	}
}

int	handle_signal_interruption(pid_t *pids, t_exec_state *state,
	t_command_data *data, t_shell *shell)
{
	int	i;

	(void)shell;
	if (g_signal != 0)
	{
		i = 0;
		while (i < data->num_commands)
		{
			if (pids[i] > 0)
				kill(pids[i], SIGINT);
			i++;
		}
		if (state->prev_pipe_read != -1)
		{
			close(state->prev_pipe_read);
			state->prev_pipe_read = -1;
		}
		return (1);
	}
	return (0);
}

int	parent_builtin(t_command_data *data, t_exec_state *state, t_shell *shell)
{
	char	*cmd;

	shell->is_save_to_execute = true;
	if (!data || !data->commands[state->i])
		return (0);
	cmd = data->commands[state->i];
	if (data->num_commands == 1 && check_builtin(cmd)
		&& (!data->input_files || !data->input_files[state->i])
		&& (!data->num_out_redirs || data->num_out_redirs[state->i] == 0))
	{
		execute_parent(data, state, shell);
		return (1);
	}
	return (0);
}

void	execute_commands(t_command_data *data, t_shell *shell)
{
	t_exec_state	state;
	pid_t			*pids;

	state.pipefd[0] = 0;
	state.pipefd[1] = 0;
	state.prev_pipe_read = -1;
	state.i = 0;
	pids = NULL;
	if (!data || !data->commands || data->num_commands == 0)
		return ((void)(shell->exit_status = 2));
	parent_builtin(data, &state, shell);
	pids = init_execution(data, &state, shell);
	if (pids == NULL)
		return ;
	set_signals_simple(1);
	while (state.i < data->num_commands)
	{
		run_pipeline(data, &state, shell, pids);
		if (handle_signal_interruption(pids, &state, data, shell))
			break ;
	}
	finalize_execution(&state, pids, data, shell);
	set_signals_interactive();
}
