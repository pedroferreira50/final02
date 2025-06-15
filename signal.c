/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:24:26 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 04:58:20 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_signal = 0;

/// @brief Interactive mode signal handler for SIGINT (Ctrl+C) - resets prompt
/// @param signo Signal number (unused, always SIGINT)
void	signal_reset_prompt(int signo)
{
	(void)signo;
	g_signal = SIGINT;
	write(1, "^C\n", 3);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/// @brief Command execution signal handler for SIGINT/SIGQUIT
/// - sets global flag
/// @param signal Signal number (SIGINT=2 for Ctrl+C, SIGQUIT=3 for Ctrl+\)
void	signal_simple_interrupt(int signal)
{
	g_signal = signal;
	if (signal == SIGINT)
		write(1, "\n", 1);
	else if (signal == SIGQUIT)
		write(1, "\n", 1);
}

/// @brief Configures SIGQUIT to be ignored (SIG_IGN) for interactive mode
void	ignore_sigquit(void)
{
	struct sigaction	act;

	memset(&act, 0, sizeof(act));
	act.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &act, NULL);
}

/// @brief Configures signals for interactive shell mode (readline prompt)
/// Sets SIGINT to reset prompt, SIGQUIT ignored
void	set_signals_interactive(void)
{
	struct sigaction	act;

	ignore_sigquit();
	memset(&act, 0, sizeof(act));
	act.sa_handler = &signal_reset_prompt;
	sigaction(SIGINT, &act, NULL);
}

/// @brief Configures signals for command execution mode
/// @param handle_sigquit 1=handle SIGQUIT, 0=ignore SIGQUIT (for heredoc)
/// Sets both SIGINT and optionally SIGQUIT to simple interrupt handler
void	set_signals_simple(int handle_sigquit)
{
	struct sigaction	act;

	if (!handle_sigquit)
		ignore_sigquit();
	memset(&act, 0, sizeof(act));
	act.sa_handler = &signal_simple_interrupt;
	sigaction(SIGINT, &act, NULL);
	if (handle_sigquit)
		sigaction(SIGQUIT, &act, NULL);
}
