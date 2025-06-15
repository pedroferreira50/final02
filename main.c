/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 12:05:30 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/15 03:37:56 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/// @brief Allocates and initializes command data structure
/// from parsed arguments
/// @param filtered_args Expanded and filtered argument array after
/// variable expansion
/// @param shell Global shell state for error handling
/// @return Pointer to populated t_command_data structure, NULL on
/// failure or empty input
static t_command_data	*prepare_command_data(char **filtered_args,
			t_shell *shell)
{
	t_command_data	*data;
	int				count;

	if (!filtered_args || !filtered_args[0])
	{
		shell->exit_status = 0;
		return (NULL);
	}
	count = 0;
	while (filtered_args[count])
		count++;
	data = malloc(sizeof(t_command_data));
	if (!data)
		return (NULL);
	ft_memset(data, 0, sizeof(t_command_data));
	parse_input(filtered_args, count, data, shell);
	return (data);
}

/// @brief Processes a complete command line through parsing, expansion,
/// and execution
/// @param input Raw command line string from user input
/// @param shell Global shell state for variable expansion and command
/// execution
void	handle_command(char *input, t_shell *shell)
{
	t_parse_result	parsed;
	t_command_data	*data;
	char			**filtered_args;
	int				pre_parse_exit_status;

	filtered_args = parse_and_expand_command(input, shell, &parsed);
	if (!filtered_args)
		return ;
	pre_parse_exit_status = shell->exit_status;
	data = prepare_command_data(filtered_args, shell);
	cleanup_parse_data(&parsed, NULL, filtered_args);
	if (!data)
		return ;
	if (pre_parse_exit_status == 0 && shell->exit_status == 1
		&& data->num_pipes == 0)
	{
		cleanup_command_data(data);
		return ;
	}
	execute_command_pipeline(data, shell);
}

/// @brief Validates and categorizes user input for appropriate processing
/// @param input Raw input string from readline
/// @param shell Global shell state for variable assignment handling
/// @return 0=EOF/exit, 1=variable assignment, 2=command to execute
int	process_input(char *input, t_shell *shell)
{
	if (input == NULL)
		return (0);
	if (input[0] == '\0')
	{
		return (1);
	}
	add_history(input);
	if (is_var_assignment(input))
	{
		handle_var_assignment(input, shell);
		return (1);
	}
	return (2);
}

/// @brief Main interactive shell loop handling
/// user input and signal processing
/// @param shell Global shell state for command processing and exit
/// status tracking
static void	run_shell_loop(t_shell *shell)
{
	char	*input;
	int		result;

	while (1)
	{
		input = readline("minishell> ");
		if (g_signal == SIGINT || g_signal == SIGQUIT)
		{
			shell->exit_status = 130;
			if (g_signal == SIGQUIT)
				shell->exit_status = 131;
			g_signal = 0;
		}
		result = process_input(input, shell);
		if (result == 0)
		{
			ft_putstr_fd("exit\n", STDOUT_FILENO);
			free(input);
			break ;
		}
		if (input && input[0] != '\0' && result == 2)
			handle_command(input, shell);
		free(input);
	}
}

/// @brief Minishell entry point - initializes shell
/// and starts interactive loop
/// @param argc Argument count (unused)
/// @param argv Argument values (unused)
/// @param envp Environment variables array passed to shell
/// @return Shell exit status from last executed command
int	main(int argc, char *argv[], char *envp[])
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	init_shell(&shell, envp);
	rl_catch_signals = 0;
	set_signals_interactive();
	run_shell_loop(&shell);
	finalize_shell(&shell);
	return (shell.exit_status);
}
