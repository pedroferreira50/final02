/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 14:58:37 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 14:21:26 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/// @brief Performs cleanup and exits child process with specified code
/// @param data Command data structure to free
/// @param pids PID array to free
/// @param shell Shell state to clean up (environment and variables)
/// @param exit_code Exit status for child process
void	cleanup_and_exit(t_command_data *data, pid_t *pids, t_shell *shell,
		int exit_code)
{
	cleanup_command_data(data);
	free(pids);
	finalize_shell(shell);
	exit(exit_code);
}

/// @brief Manages parent process operations after
/// fork during pipeline execution
/// @param pid Child process ID from fork
/// @param pids Array to store all child process IDs
/// @param state Execution state with pipe file descriptors and counters
/// @param data Command data structure with pipeline information
void	manage_parent(pid_t pid, pid_t *pids, t_exec_state *state,
	t_command_data *data)
{
	if (pid > 0)
	{
		pids[state->i] = pid;
		if (state->prev_pipe_read != -1)
		{
			close(state->prev_pipe_read);
			state->prev_pipe_read = -1;
		}
		if (state->i < data->num_commands - 1)
		{
			state->prev_pipe_read = state->pipefd[0];
			close(state->pipefd[1]);
		}
	}
}

/// @brief Checks if PATH environment variable is unset or empty
/// @param shell Global shell state containing environment variables
/// @return 1 if PATH is unset or empty string, 0 if PATH has value
static int	path_is_unset_or_empty(t_shell *shell)
{
	int	i;

	i = 0;
	while (shell->envp && shell->envp[i])
	{
		if (ft_strncmp(shell->envp[i], "PATH=", 5) == 0)
		{
			if (shell->envp[i][5] == '\0')
				return (1);
			return (0);
		}
		i++;
	}
	return (1);
}

/// @brief Executes single command in pipeline with
/// path resolution and error handling
/// @param i Current command index in pipeline
/// @param shell Global shell state for environment and error reporting
/// @param pids Process ID array for cleanup on error
/// @param data Command data containing command and argument arrays
void	execute_command(int *i, t_shell *shell, pid_t *pids,
	t_command_data *data)
{
	char		*full_path;
	struct stat	sb;

	full_path = find_command_path(data->commands[*i], shell);
	if (full_path == NULL)
	{
		if (!ft_strchr(data->commands[*i], '/') && path_is_unset_or_empty(shell)
			&& stat(data->commands[*i], &sb) == 0 && !S_ISDIR(sb.st_mode))
		{
			print_error_command(data->commands[*i], "Permission denied");
			g_signal = 126;
			shell->exit_status = 126;
			cleanup_and_exit(data, pids, shell, 126);
		}
		print_error_command(data->commands[*i], "command not found");
		g_signal = 127;
		shell->exit_status = 127;
		cleanup_and_exit(data, pids, shell, 127);
	}
	execve(full_path, data->arguments[*i], shell->envp);
	perror("execve");
	free(full_path);
	cleanup_and_exit(data, pids, shell, shell->exit_status);
}

/// @brief Runs single command in pipeline with fork and pipe management
/// @param data Command data structure with pipeline information
/// @param state Execution state tracking pipes and current command
/// @param shell Global shell state for environment and error handling
/// @param pids Process ID array for child process tracking
void	run_pipeline(t_command_data *data, t_exec_state *state,
	t_shell *shell, pid_t *pids)
{
	pid_t	pid;

	if (setup_pipeline(data, state, shell) == -1)
	{
		if (g_signal != SIGINT)
		{
			shell->exit_status = 1;
			g_signal = 0;
		}
		return ;
	}
	pid = fork();
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		fork_child(data, state, shell, pids);
	}
	if (pid < 0)
	{
		perror("minishell: fork");
		shell->exit_status = 1;
		return ;
	}
	manage_parent(pid, pids, state, data);
	state->i++;
}
