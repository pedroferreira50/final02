/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 15:03:27 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 16:59:51 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	handle_wait_status(int status, t_shell *shell)
{
	if (WIFEXITED(status))
	{
		shell->exit_status = WEXITSTATUS(status);
	}
	else
	{
		if (WIFSIGNALED(status))
		{
			shell->exit_status = 128 + WTERMSIG(status);
			if (shell->exit_status == 139)
				print_error_simple("segmentation fault", 139, shell);
		}
	}
}

void	wait_commands(pid_t *pids, t_command_data *data, t_shell *shell)
{
	int	i;
	int	status;

	i = 0;
	while (i < data->num_commands)
	{
		if (pids[i] > 0)
		{
			if (waitpid(pids[i], &status, 0) > 0)
				handle_wait_status(status, shell);
		}
		i = i + 1;
	}
	if (pids)
	{
		free(pids);
		pids = NULL;
	}
}

void	fork_child(t_command_data *data, t_exec_state *state,
				t_shell *shell, pid_t *pids)
{
	int	has_builtin;

	if (data->commands[state->i] == NULL)
		exit(0);
	has_builtin = check_builtin(data->commands[state->i]);
	setup_pipes_and_redirections(data, state, data->num_commands, shell);
	if (state->i < data->num_commands - 1)
		close(state->pipefd[0]);
	if (has_builtin && shell->is_save_to_execute == true)
	{
		signal(SIGPIPE, SIG_IGN);
		child_builtin(&state->i, shell, data);
		cleanup_command_data(data);
		free_args(shell->envp, NULL);
		free_all_vars(&shell->vars);
		free(pids);
		exit(shell->exit_status);
	}
	if (!has_builtin)
		execute_command(&state->i, shell, pids, data);
	cleanup_command_data(data);
	free_args(shell->envp, NULL);
	free_all_vars(&shell->vars);
	free(pids);
	exit(1);
}

void	finalize_execution(t_exec_state *state, pid_t *pids,
	t_command_data *data, t_shell *shell)
{
	int	save_exit;

	save_exit = shell->exit_status;
	wait_commands(pids, data, shell);
	if (g_signal == SIGINT)
		shell->exit_status = 130;
	else if (g_signal == SIGQUIT)
		shell->exit_status = 131;
	else if (!shell->is_save_to_execute)
		shell->exit_status = save_exit;
	shell->is_save_to_execute = true;
	if (state->prev_pipe_read != -1)
	{
		close(state->prev_pipe_read);
		state->prev_pipe_read = -1;
	}
}
